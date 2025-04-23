#include "minishell.h"

int handle_output_redirection(t_redir *redir)
{
    int fd;

    if (redir->type == TOKEN_REDIRECT_OUT)
    {
        fd = open(redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1)
        {
            perror("open");
            return -1;
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
    else if (redir->type == TOKEN_APPEND)
    {
        fd = open(redir->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd == -1)
        {
            perror("open");
            return -1;
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
    return (0);
}

int handle_input_redirection(t_redir *redir)
{
    int fd;

    if (redir->type == TOKEN_REDIRECT_IN)
    {
        fd = open(redir->file, O_RDONLY);
        if (fd == -1)
            return (perror(redir->file), -1);
        dup2(fd, STDIN_FILENO);
        close(fd);
    }
    else if (redir->type == TOKEN_HDC)
    {
        if (redir->fd != -1)
        {
            dup2(redir->fd, STDIN_FILENO);
            close(redir->fd);
            redir->fd = -1;
        }
        else
            return (perror("heredoc not processed"), -1);
    }
    return (0);
}
