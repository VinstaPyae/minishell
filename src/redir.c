#include "minishell.h"

t_list *create_redir(char *file, int type)
{
    t_redir *c_redir;
    t_list *redir;

    // Allocate memory for the redirection structure
    c_redir = malloc(sizeof(t_redir));
    if (c_redir == NULL)
        return NULL;

    // Handle the case where file is NULL
    if (file == NULL)
        c_redir->file = NULL;
    else
    {
        c_redir->file = ft_strdup(file);
        if (c_redir->file == NULL)
        {
            free(c_redir);
            return NULL;
        }
    }
    c_redir->type = type;
    c_redir->fd = -1;
    redir = ft_lstnew(c_redir);
    if (redir == NULL)
        return (free(c_redir), NULL);
    redir->next = NULL;
    return (redir);
}

void close_heredoc_fds(t_ast_node *node)
{
    if (!node)
        return;

    // Close heredoc FDs in the current node's redirections
    if (node->redir)
    {
        t_list *current = node->redir;
        while (current)
        {
            t_redir *redir = (t_redir *)current->content;
            if (redir->type == TOKEN_HDC && redir->fd != -1)
            {
                close(redir->fd);
                redir->fd = -1; // Mark as closed
            }
            current = current->next;
        }
    }

    // Recursively process pipe nodes
    if (node->type == NODE_PIPE)
    {
        close_heredoc_fds(node->left);
        close_heredoc_fds(node->right);
    }
}

void print_redir(t_list *redir)
{
    t_list *current = redir;
    while (current != NULL)
    {
        t_redir *r = (t_redir *)current->content;
        printf("Redir Type: %d, File: %s\n", r->type, r->file);
        current = current->next;
    }
}

// Modified process_heredocs function
int process_heredocs(t_ast_node *node, t_minishell *shell)
{
    if (!node)
        return (0);

    if (node->redir)
    {
        t_list *current = node->redir;
        while (current)
        {
            t_redir *redir = (t_redir *)current->content;
            if (redir->type == TOKEN_HDC)
            {
                redir->fd = handle_heredoc(redir->file, shell);
                if (redir->fd == -1)
                    return (close_heredoc_fds(node),-1);
            }
            current = current->next;
        }
    }

    if (node->type == NODE_PIPE)
    {
        if (process_heredocs(node->left, shell) == -1 || process_heredocs(node->right, shell) == -1)
            return (close_heredoc_fds(node),-1);
    }

    return (0);
}
char *append_expanded_heredoc(char *result, char *var_name, t_minishell *shell)
{
    char *name = var_name + 1;   // skip the '$'
    char *expanded;
    char *value;

    if (strcmp(name, "?") == 0) {
        // special case: exit status
        expanded = ft_itoa(shell->exit_status);
    } else {
        // lookup in envp
        value = ft_getenv(shell->envp, name);
        if (value)
            expanded = ft_strdup(value);
        else
            expanded = ft_strdup("");   // safe: expand undefined vars to empty
    }

    // join onto the growing result
    char *temp = ft_strjoin(result, expanded);
    free(result);
    free(expanded);
    free(var_name);
    return temp;
}


char *extract_heredoc_name(const char *input, int *i) {
    int start = *i;
    int j = start + 1; // Start after '$'

    // Handle $? and $digit immediately
    if (input[j] == '?') {
        j++;
    } else if (ft_isdigit(input[j])) {
        j++; // $1, $2, etc. (only capture the digit)
    } else {
        // Check if valid variable start (letter or underscore)
        if (ft_isalpha(input[j]) || input[j] == '_') {
            j++;
            // Continue until non-alphanumeric/underscore
            while (ft_isalnum(input[j]) || input[j] == '_') {
                j++;
            }
        } else {
            // Not a valid variable; treat as literal '$'
            j = start + 1;
        }
    }

    char *var_name = ft_substr(input, start, j - start);
    *i = j; // Update index to end of variable
    return var_name;
}
    // Main function
char *expand_heredoc(char *input, t_minishell *shell) {
    int i = 0;
    char *result = ft_strdup(""); // Start empty

    while (input[i]) {
        if (input[i] == '$') {
            char *var_name = extract_heredoc_name(input, &i);
            printf("var_name: (%s)\n", var_name);

            // Expand if valid variable (length > 1, e.g., "$VAR")
            if (ft_strlen(var_name) > 1) {
                result = append_expanded_heredoc(result, var_name, shell);
            } else {
                // Append lone '$' as literal
                result = append_normal_character(result, '$');
            }
        } else {
            // Append normal character
            result = append_normal_character(result, input[i]);
            i++;
        }
    }
    return result;
}

// Modified handle_heredoc function
int handle_heredoc(char *delimiter, t_minishell *shell)
{
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return -1;
    }

    struct sigaction old_sa, new_sa;
    sigaction(SIGINT, NULL, &old_sa);
    new_sa = old_sa;
    new_sa.sa_handler = handle_sigint_heredoc;
    new_sa.sa_flags &= ~SA_RESTART;
    sigaction(SIGINT, &new_sa, NULL);

    g_signal_status = 0;
    rl_event_hook = check_sigint;

    char *line;
    char *expanded_line = NULL;
    int eof_warning = 0;
    while (1)
    {
        line = readline("> ");
        if (!line)
        {
            if (!eof_warning)
            {
                fprintf(stderr, "minishell: warning: here-document delimited by end-of-file (wanted `%s')\n", delimiter);
                eof_warning = 1;
            }
            break;
        }
        expanded_line = expand_heredoc(line, shell);
        // printf("Expanded line: %s\n", expanded_line);
        if (g_signal_status == 130)
        {
            free(line);
            free(expanded_line);
            break;
        }
        if (strcmp(line, delimiter) == 0)
        {
            free(line);
            free(expanded_line);
            break;
        }
        write(pipefd[1], expanded_line, strlen(expanded_line));
        write(pipefd[1], "\n", 1);
        free(line);
        free(expanded_line);
    }

    close(pipefd[1]);
    sigaction(SIGINT, &old_sa, NULL);
    rl_event_hook = NULL;

    if (g_signal_status == 130)
    {
        close(pipefd[0]);
        return -1;
    }
    return pipefd[0];
}

int handle_redirections(t_list *redir_list)
{
    t_redir *redir;
    int fd;

    while (redir_list)
    {
        redir = (t_redir *)redir_list->content;
        if (redir->type == TOKEN_REDIRECT_OUT)
        {
            fd = open(redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1)
                return (perror("open"), -1);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        else if (redir->type == TOKEN_APPEND)
        {
            fd = open(redir->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd == -1)
                return (perror("open"), -1);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        else if (redir->type == TOKEN_REDIRECT_IN)
        {
            fd = open(redir->file, O_RDONLY);
            if (fd == -1)
                return (perror("open"), -1);
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        else if (redir->type == TOKEN_HDC)
        {
            if (redir->fd != -1)
            {
                dup2(redir->fd, STDIN_FILENO);
                close(redir->fd); // Close after duplication
                redir->fd = -1;   // Mark as closed
            }
            else
                return (perror("heredoc not processed"), -1);
        }
        redir_list = redir_list->next;
    }
    return 0;
}
