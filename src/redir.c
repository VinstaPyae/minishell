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
int process_heredocs(t_ast_node *node)
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
                redir->fd = handle_heredoc(redir->file);
                if (redir->fd == -1)
                    return (close_heredoc_fds(node),-1);
            }
            current = current->next;
        }
    }

    if (node->type == NODE_PIPE)
    {
        if (process_heredocs(node->left) == -1 || process_heredocs(node->right) == -1)
            return (close_heredoc_fds(node),-1);
    }

    return (0);
}

// Modified handle_heredoc function
int handle_heredoc(char *delimiter)
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
        if (g_signal_status == 130)
        {
            free(line);
            break;
        }
        if (strcmp(line, delimiter) == 0)
        {
            free(line);
            break;
        }
        write(pipefd[1], line, strlen(line));
        write(pipefd[1], "\n", 1);
        free(line);
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
