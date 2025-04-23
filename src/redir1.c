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
