#include "minishell.h"
#include <signal.h>

char *ft_getenv(t_env *env, const char *key)
{
    while (env)
    {
        if (ft_strcmp(env->key, key) == 0)
            return env->value;
        env = env->next;
    }
    return NULL;
}

void close_og_fd(t_minishell *shell)
{
    close(shell->og_fd[1]);
    close(shell->og_fd[0]);
}

static char **allocate_paths(char *path_env)
{
    int count;
    char **path_dirs;

    count = 1;
    if (!path_env) // Handle NULL case
        return (NULL);
    while (*path_env)
    {
        if (*path_env == ':')
            count++;
        path_env++;
    }
    path_dirs = malloc((count + 1) * sizeof(char *));
    if (!path_dirs)
        return (NULL);
    return (path_dirs);
}

char **split_path(char *path_env)
{
    char **path_dirs;
    char *start;
    int index;
    int count;

    path_dirs = allocate_paths(path_env);
    if (!path_dirs)
        return (NULL);
    start = path_env;
    index = 0;
    while (*path_env)
    {
        if (*path_env == ':' || !*(path_env + 1))
        {
            count = path_env - start + (!*(path_env + 1));
            path_dirs[index] = ft_strndup(start, count);
            if (!path_dirs[index])
                return (free_array_list(path_dirs, index), NULL);
            index++;
            start = path_env + 1;
        }
        path_env++;
    }
    path_dirs[index] = NULL;
    return (path_dirs);
}
////////////////////////////////
char *create_env_entry(char *key, char *value)
{
    char *entry;
    size_t key_len;
    size_t value_len;
    size_t total_len;

    key_len = ft_strlen(key);
    value_len = ft_strlen(value);
    total_len = key_len + value_len + 2; // +1 for '=', +1 for '\0'
    entry = malloc(total_len);
    if (!entry)
        return (NULL);

    ft_memcpy(entry, key, key_len);
    entry[key_len] = '=';
    ft_memcpy(entry + key_len + 1, value, value_len);
    entry[total_len - 1] = '\0';
    return (entry);
}

char **fill_env_array(char **env_array, t_env *tmp, int i)
{

    while (tmp != NULL)
    {
        if (tmp->key != NULL && tmp->value != NULL)
        {
            env_array[i] = create_env_entry(tmp->key, tmp->value);
            if (env_array[i] == NULL)
            {
                free_array_list(env_array, i);
                return (NULL);
            }
            i++;
        }
        tmp = tmp->next;
    }
    env_array[i] = NULL;
    return (env_array);
}

char **env_list_to_array(t_env *env)
{
    int count;
    t_env *tmp;
    char **env_array;
    int i;

    count = 0;
    tmp = env;
    i = 0;
    while (tmp != NULL)
    {
        if (tmp->key != NULL && tmp->value != NULL)
            count++;
        tmp = tmp->next;
    }
    env_array = malloc((count + 1) * sizeof(char *));
    if (env_array == NULL)
        return (NULL);
    tmp = env;
    env_array = fill_env_array(env_array, tmp, i);
    return (env_array);
}
///////////

void handle_child_signals(void)
{
    struct sigaction sa;

    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGTSTP, &sa, NULL);
}

void print_error_message(char *cmd, char *message)
{
    write(STDERR_FILENO, cmd, ft_strlen(cmd));
    write(STDERR_FILENO, ": ", 2);
    write(STDERR_FILENO, message, ft_strlen(message));
    write(STDERR_FILENO, "\n", 1);
}

int is_directory(char *cmd, t_minishell *shell)
{
    struct stat sb;

    if (stat(cmd, &sb) == 0 && S_ISDIR(sb.st_mode))
    {
        print_error_message(cmd, "Is a directory");
        set_exit_status(shell, 126);
        return (1);
    }
    return (0);
}

char **get_env_array(t_minishell *shell)
{
    char **env_array;

    env_array = env_list_to_array(shell->envp);
    if (!env_array)
    {
        perror("env_list_to_array");
        set_exit_status(shell, 127);
    }
    return (env_array);
}
int handle_no_path(char *cmd, t_minishell *shell)
{
    print_error_message(cmd, "No such file or directory");
    return (return_with_status(shell, 127));
}

void print_signal_message(int sig)
{
    if (isatty(STDERR_FILENO))
    {
        if (sig == SIGQUIT)
            write(STDERR_FILENO, "Quit\n", 4);
    }
    write(STDERR_FILENO, "\n", 1);
}

/* Wait for child and handle status */
static int wait_for_child(pid_t pid)
{
    int status;
    int ret = 0;

    if (waitpid(pid, &status, 0) == -1)
    {
        perror("minishell: waitpid");
        return -1;
    }

    if (WIFSIGNALED(status))
    {
        int sig = WTERMSIG(status);
        ret = 128 + sig;
        if (sig == SIGINT || sig == SIGQUIT)
        {
            print_signal_message(sig);
        }
    }
    else if (WIFEXITED(status))
    {
        ret = WEXITSTATUS(status);
    }

    return ret;
}

/* Helper function to create a child process for pipe execution */
static pid_t create_pipe_child(t_ast_node *node, t_minishell *shell, int *pipe_fd, int *og_fd)
{
    pid_t pid;

    if (!node && !node->left)
    {
        return -1;
    }
    pid = fork();
    if (pid < 0)
    {
        perror("minishell: fork");
        return -1;
    }

    if (pid == 0)
    {
        /* Child process */
        handle_child_signals();
        reset_close_fd(og_fd, 0, 1);
        /* Set up input redirection */
        if (node->right)
        {
            dup2(pipe_fd[1], STDOUT_FILENO);
        }
	    close(pipe_fd[0]);
        close(pipe_fd[1]);
        // print_ast_node(node);
        int ret = exe_cmd(node, og_fd, shell);
        //printf("Child process finished executing command: %s\n", node->cmd_arg[0]);
        // print_ast_node(node); // Print AST node for debugging
        cleanup(&shell);
        free_env_list(shell->envp);
        if (shell)
            free(shell);
        exit(ret);
    }
    else
    {
        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[1]);
        close(pipe_fd[0]);
    }
    // printf("Created child process with PID: %d\n", pid);
    // // print_ast_node(node); // Print AST node for debugging
    return pid;
}

/* Recursive pipe execution with proper fd handling */
int execute_pipe(t_ast_node *pipe_node, int *og_fd, t_minishell *shell)
{
    int pipe_fds[2];
    pid_t left_pid;
    pid_t right_pid;
    int exit_status = 0;
    if (!pipe_node)
    {
	return -1;
    }

    if (pipe(pipe_fds) == -1)
    {
        perror("minishell: pipe Failed");
        return -1;
    }

    g_signal_status = 2; // Mark child processes running

    /* Execute left side (writes to pipe) */
    printf("Executing left side of pipe\n");
    left_pid = create_pipe_child(pipe_node->left, shell, pipe_fds, og_fd);
    if (pipe_node->right->type == NODE_PIPE)
    {
        execute_pipe(pipe_node->right, og_fd, shell);
        exit_status = wait_for_child(left_pid);
    }
    else if (pipe_node->right->type == NODE_COMMAND)
    {
        right_pid = create_pipe_child(pipe_node->right, shell, pipe_fds, og_fd);
        exit_status = wait_for_child(right_pid);
    }
    else
    {
        exit_status = wait_for_child(left_pid); 
    }
    // close(pipe_fds[0]);
    // printf("Pipe closed\n");
    // print_ast_node(pipe_node); // Print current AST node for debugging
    // /* Wait for left process */
    // int left_status = wait_for_child(left_pid);
    // // cleanup(&shell); // Cleanup after left process
    // (void)left_status; // We return rightmost command's status
    g_signal_status = 0;
    return exit_status;
}

///////////////////////////****************** //////////////////

void reset_close_fd(int *org_fd, int reset, int closee)
{
    if (reset)
    {
        dup2(org_fd[0], STDIN_FILENO);
        dup2(org_fd[1], STDOUT_FILENO);
    }
    if (closee)
    {
        close(org_fd[0]);
        close(org_fd[1]);
    }
}

int execute_ast(t_minishell *shell)
{
    int og_fd[2];
    t_ast_node *ast_nd;
    ast_nd = (shell)->ast;
    int result = 0;

    if (!ast_nd)
    {
        perror("minishell: No AST to execute");
        return 1;
    }
    og_fd[0] = dup(STDIN_FILENO);
    og_fd[1] = dup(STDOUT_FILENO);

    /* Process heredocs */
    if (process_heredocs(ast_nd) == -1)
    {
        (shell)->exit_status = 130;
        close((shell)->og_fd[0]);
        close((shell)->og_fd[1]);
        return 1;
    }
//     print_ast_node(ast_nd); // Print AST node for debugging
    if (ast_nd && ast_nd->type == NODE_PIPE)
    {
        result = execute_pipe(ast_nd, og_fd, shell);
        if (result == -1)
        {
            perror("minishell: execute_pipe");
            return 1;
        }
    }
    else if (ast_nd && ast_nd->type == NODE_COMMAND)
    {
        result = exe_cmd(ast_nd, og_fd, shell);
        if (result == -1)
        {
            perror("minishell: execute_ast_command");
            return 1;
        }
    }
    reset_close_fd(og_fd, 1, 1);
    cleanup(&shell);
    return result;
}