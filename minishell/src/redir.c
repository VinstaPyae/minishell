#include "minishell.h"

t_list *create_redir(char *file, int type)
{
	t_redir *c_redir;
	t_list	*redir;

	// Allocate memory for the redirection structure
	c_redir = malloc(sizeof(t_redir));
	if (c_redir == NULL)
		return NULL;

	// Handle the case where file is NULL
	if (file == NULL)
		c_redir->file = NULL;
	else {
		c_redir->file = ft_strdup(file);
		if (c_redir->file == NULL) {
		free(c_redir);
		return NULL;
		}
	}
	c_redir->type = type;
	redir = ft_lstnew(c_redir);
	if (redir == NULL)
		return (free(c_redir),NULL);
	redir->next = NULL;
	return (redir);
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

int handle_heredoc(char *delimiter)
{
	int pipefd[2];
	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return -1;
	}
	
	char *line;
	while (1)
	{
		// You might use readline() from the GNU Readline library
		line = readline("> ");
		if (!line || strcmp(line, delimiter) == 0)
		{
		free(line);
		break;
		}
		write(pipefd[1], line, strlen(line));
		write(pipefd[1], "\n", 1);
		free(line);
	}
	close(pipefd[1]); // Close the write end after writing
	return pipefd[0]; // Return the read end to be used as STDIN
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
			int heredoc_fd = handle_heredoc(redir->file);
				if (heredoc_fd == -1)
					return (perror("open"), -1);
			dup2(heredoc_fd, STDIN_FILENO);
			close(heredoc_fd);
		}
		redir_list = redir_list->next;
    }
    return 0;
}
