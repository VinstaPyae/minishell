#include "minishell.h"

t_list *create_redir(char *file, int type)
{
	t_redir *c_redir;
	t_list *redir;

	c_redir = malloc(sizeof(t_redir));
	if (c_redir == NULL)
		return NULL;
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
	t_list *current;
	t_redir *redir;

	if (!node)
		return;
	if (node->redir)
	{
		current = node->redir;
		while (current)
		{
			redir = (t_redir *)current->content;
			if (redir->type == TOKEN_HDC && redir->fd != -1)
			{
				close(redir->fd);
				redir->fd = -1; // Mark as closed
			}
			current = current->next;
		}
	}
	if (node->type == NODE_PIPE)
	{
		close_heredoc_fds(node->left);
		close_heredoc_fds(node->right);
	}
}

void print_redir(t_list *redir)
{
	t_list *current;
	t_redir *r;

	current = redir;;
	while (current != NULL)
	{
		r = (t_redir *)current->content;
		printf("Redir Type: %d, File: %s\n", r->type, r->file);
		current = current->next;
	}
}

// Modified process_heredocs function
int process_heredocs(t_ast_node *node, t_minishell *shell)
{
	t_list *current;
	t_redir *redir;

	if (!node)
		return (0);
	if (node->redir)
	{
		current = node->redir;
		while (current)
		{
			redir = (t_redir *)current->content;
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
		if (process_heredocs(node->left, shell) == -1
			|| process_heredocs(node->right, shell) == -1)
			return (close_heredoc_fds(node), -1);
	return (0);
}
char *append_expanded_heredoc(char *result, char *var_name, t_minishell *shell)
{
	char *name;
	char *expanded;
	char *value;
	char *temp;

	name = var_name + 1;
	if (ft_strcmp(name, "?") == 0)
		expanded = ft_itoa(shell->exit_status);
	else
	{
		value = ft_getenv(shell->envp, name);
		if (value)
			expanded = ft_strdup(value);
		else
			expanded = ft_strdup("");   // safe: expand undefined vars to empty
	}
	temp = ft_strjoin(result, expanded);
	free(result);
	free(expanded);
	free(var_name);
	return (temp);
}


char *extract_heredoc_name(const char *input, int *i)
{
	char *var_name;
	int start;
	int j;

	start = *i;
	j = start + 1;
	if (input[j] == '?')
		j++;
	else if (ft_isdigit(input[j]))
		j++;
	else
	{
		if (ft_isalpha(input[j]) || input[j] == '_')
		{
			j++;
			while (ft_isalnum(input[j]) || input[j] == '_')
				j++;
		}
		else
			j = start + 1;
	}
	var_name = ft_substr(input, start, j - start);
	*i = j;
	return (var_name);
}

char *expand_heredoc(char *input, t_minishell *shell)
{
	char *result;
	char *var_name;
	int i;

	i = 0;
	result = ft_strdup("");
	while (input[i])
	{
		if (input[i] == '$')
		{
			var_name = extract_heredoc_name(input, &i);
			if (ft_strlen(var_name) > 1)
				result = append_expanded_heredoc(result, var_name, shell);
			else
				result = append_normal_character(result, '$');
		}
		else
		{
			result = append_normal_character(result, input[i]);
			i++;
		}
	}
	return (result);
}

// Modified handle_heredoc function
// ...existing code...
// Function to handle the expansion of a heredoc line
char *process_heredoc_line(char *line, char *delimiter, t_minishell *shell, int *should_break)
{
	char *expanded_line;

	expanded_line = expand_heredoc(line, shell);
	if (g_signal_status == 130) // Signal interrupt
	{
		free(line);
		free(expanded_line);
		*should_break = 1;
		return NULL;
	}
	if (ft_strcmp(line, delimiter) == 0) // End of heredoc
	{
		free(line);
		free(expanded_line);
		*should_break = 1;
		return NULL;
	}
	free(line);
	return expanded_line;
}

// Function to write the expanded line to the pipe
void write_expanded_line_to_pipe(char *expanded_line, int pipefd)
{
	if (expanded_line)
	{
		write(pipefd, expanded_line, ft_strlen(expanded_line));
		write(pipefd, "\n", 1);
		free(expanded_line);
	}
}

void setup_heredoc_signals(struct sigaction *old_sa)
{
	struct sigaction new_sa;

	sigaction(SIGINT, NULL, old_sa);
	new_sa = *old_sa;
	new_sa.sa_handler = handle_sigint_heredoc;
	new_sa.sa_flags &= ~SA_RESTART;
	sigaction(SIGINT, &new_sa, NULL);
	rl_event_hook = check_sigint;
	g_signal_status = 0;
}


void restore_heredoc_signals(struct sigaction *old_sa)
{
	sigaction(SIGINT, old_sa, NULL);
	rl_event_hook = NULL;
}

static void process_eof_warning(char *delimiter)
{
	write(STDERR_FILENO, "minishell: warning: here-document delimited by end-of-file (wanted `", 68);
	write(STDERR_FILENO, delimiter, ft_strlen(delimiter));
	write(STDERR_FILENO, "')\n", 3);
}

int read_heredoc_loop(char *delimiter, t_minishell *shell, int write_fd)
{
	char *line;
	char *expanded_line;
	int eof_warning;
	int should_break;

	eof_warning = 0;
	should_break = 0;
	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			if (!eof_warning)
			{
			   process_eof_warning(delimiter);
				eof_warning = 1;
			}
			break;
		}
		expanded_line = process_heredoc_line(line, delimiter, shell, &should_break);
		if (should_break)
			break;
		write_expanded_line_to_pipe(expanded_line, write_fd);
	}
	return 0;
}
int handle_heredoc(char *delimiter, t_minishell *shell)
{
	int pipefd[2];
	struct sigaction old_sa;
	int result;

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return -1;
	}
	setup_heredoc_signals(&old_sa);
	read_heredoc_loop(delimiter, shell, pipefd[1]);
	close(pipefd[1]);
	restore_heredoc_signals(&old_sa);
	if (g_signal_status == 130)
	{
		close(pipefd[0]);
		return -1;
	}
	result = pipefd[0];
	return result;
}
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

int handle_redirections(t_list *redir_list)
{
    t_redir *redir;
    int result;

    while (redir_list)
    {
        redir = (t_redir *)redir_list->content;
        if (redir->type == TOKEN_REDIRECT_OUT || redir->type == TOKEN_APPEND)
        {
            result = handle_output_redirection(redir);
            if (result == -1)
                return -1;
        }
        else if (redir->type == TOKEN_REDIRECT_IN || redir->type == TOKEN_HDC)
        {
            result = handle_input_redirection(redir);
            if (result == -1)
                return -1;
        }
        redir_list = redir_list->next;
    }
    return (0);
}



// // // Refactored handle_heredoc function
// int handle_heredoc(char *delimiter, t_minishell *shell)
// {
//     int pipefd[2];
//     if (pipe(pipefd) == -1)
//     {
//         perror("pipe");
//         return -1;
//     }

//     struct sigaction old_sa, new_sa;
//     sigaction(SIGINT, NULL, &old_sa);
//     new_sa = old_sa;
//     new_sa.sa_handler = handle_sigint_heredoc;
//     new_sa.sa_flags &= ~SA_RESTART;
//     sigaction(SIGINT, &new_sa, NULL);

//     g_signal_status = 0;
//     rl_event_hook = check_sigint;

//     char *line;
//     char *expanded_line = NULL;
//     int eof_warning = 0;
//     int should_break = 0;

//     while (1)
//     {
//         line = readline("> ");
//         if (!line)
//         {
//             if (!eof_warning)
//             {
//                 write(2, "minishell: warning: here-document delimited by end-of-file (wanted `", 68);
//                 write(2, delimiter, ft_strlen(delimiter));
//                 write(2, "')\n", 3);
//                 eof_warning = 1;
//             }
//             break;
//         }

//         expanded_line = process_heredoc_line(line, delimiter, shell, &should_break);
//         if (should_break)
//             break;

//         write_expanded_line_to_pipe(expanded_line, pipefd[1]);
//     }

//     close(pipefd[1]);
//     sigaction(SIGINT, &old_sa, NULL);
//     rl_event_hook = NULL;

//     if (g_signal_status == 130)
//     {
//         close(pipefd[0]);
//         return -1;
//     }
//     return pipefd[0];
// }

// int handle_redirections(t_list *redir_list)
// {
// 	t_redir *redir;
// 	int fd;

// 	while (redir_list)
// 	{
// 		redir = (t_redir *)redir_list->content;
// 		if (redir->type == TOKEN_REDIRECT_OUT)
// 		{
// 			fd = open(redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
// 			if (fd == -1)
// 				return (perror("open"), -1);
// 			dup2(fd, STDOUT_FILENO);
// 			close(fd);
// 		}
// 		else if (redir->type == TOKEN_APPEND)
// 		{
// 			fd = open(redir->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
// 			if (fd == -1)
// 				return (perror("open"), -1);
// 			dup2(fd, STDOUT_FILENO);
// 			close(fd);
// 		}
// 		else if (redir->type == TOKEN_REDIRECT_IN)
// 		{
// 			fd = open(redir->file, O_RDONLY);
// 			if (fd == -1)
// 				return (perror(redir->file), -1);
// 			dup2(fd, STDIN_FILENO);
// 			close(fd);
// 		}
// 		else if (redir->type == TOKEN_HDC)
// 		{
// 			if (redir->fd != -1)
// 			{
// 				dup2(redir->fd, STDIN_FILENO);
// 				close(redir->fd); // Close after duplication
// 				redir->fd = -1;   // Mark as closed
// 			}
// 			else
// 				return (perror("heredoc not processed"), -1);
// 		}
// 		redir_list = redir_list->next;
// 	}
// 	return 0;
// }
