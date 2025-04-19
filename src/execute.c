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
int wait_for_child(pid_t pid)
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

/* Execute a pipe command */
int execute_pipe(t_ast_node *pipe_node, t_minishell *shell)
{
    int pipe_fds[2];
    pid_t left_pid, right_pid;
    int status;
    
    if (!pipe_node)
        return -1;
    
    /* Create the pipe */
    if (pipe(pipe_fds) == -1)
    {
        perror("minishell: pipe Failed");
        return -1;
    }
    
    g_signal_status = 2; /* Mark child processes running */
    
    /* Create left child (writes to pipe) */
    left_pid = fork();
    if (left_pid < 0)
    {
        perror("minishell: fork");
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        return -1;
    }
    
    if (left_pid == 0) /* Left child process */
    {
        //handle_child_signals();
        
        /* Close read end in left child */
        close(pipe_fds[0]);
        
        /* Redirect stdout to pipe write end */
        if (dup2(pipe_fds[1], STDOUT_FILENO) == -1)
        {
            perror("minishell: dup2");
            exit(1);
        }
        close(pipe_fds[1]);
        
        int ret = exe_cmd(pipe_node->left, shell);
        cleanup(&shell);
        free_env_list(shell->envp);
        if (shell)
            free(shell);
        exit(ret);
    }
    
    /* Create right child (reads from pipe) */
    right_pid = fork();
    if (right_pid < 0)
    {
        perror("minishell: fork");
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        waitpid(left_pid, NULL, 0); /* Clean up the left child */
        return -1;
    }
    
    if (right_pid == 0) /* Right child process */
    {
        handle_child_signals();
        
        /* Close write end in right child */
        close(pipe_fds[1]);
        
        /* Redirect stdin from pipe read end */
        if (dup2(pipe_fds[0], STDIN_FILENO) == -1)
        {
            perror("minishell: dup2");
            exit(1);
        }
        close(pipe_fds[0]);
        
        int ret;
        ret = execute_ast(pipe_node->right, shell);
        cleanup(&shell);
        free_env_list(shell->envp);
        if (shell)
            free(shell);
        exit(ret);
    }
    
    /* Parent closes both pipe ends */
    close(pipe_fds[0]);
    close(pipe_fds[1]);
    
    /* Wait for both children to finish */
    waitpid(left_pid, NULL, 0);  /* We don't care about the left exit status */
    int right_status = wait_for_child(right_pid);
    
    g_signal_status = 0;
    
    // /* Return the exit status of the right command */
    // if (WIFEXITED(status))
    //     return WEXITSTATUS(status);
    // else if (WIFSIGNALED(status))
    //     return 128 + WTERMSIG(status);
        
    return (return_with_status(shell, right_status));
}

///////////////////////////****************** //////////////////

void reset_close_fd(int *org_fd)
{
    close(org_fd[0]);
    close(org_fd[1]);
}

int execute_ast(t_ast_node *ast_root, t_minishell *shell)
{
    int result = 0;

    //(signal(SIGINT, SIG_IGN), signal(SIGQUIT, SIG_IGN));
    if (!ast_root)
    {
        perror("minishell: No AST to execute");
        return (return_with_status(shell, 1));
    }

    /* Process heredocs */
    // if (process_heredocs(ast_root) == -1)
    //     return (return_with_status(shell, 130));
//     print_ast_node(ast_nd); // Print AST node for debugging
    if (ast_root && ast_root->type == NODE_PIPE)
    {
        result = execute_pipe(ast_root, shell);
        if (result == -1)
        {
            perror("minishell: execute_pipe");
            return (return_with_status(shell, 1));
        }
    }
    else if (ast_root && ast_root->type == NODE_COMMAND)
    {
        result = exe_cmd(ast_root, shell);
        if (result == -1)
        {
            perror("minishell: execute_ast_command");
            return (return_with_status(shell, 1));
        }
    }
    setup_signal_handlers(); // Restore signal handlers for parent process
    cleanup(&shell);
    return result;
}