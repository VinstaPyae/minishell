#include "minishell.h"

t_list	*create_redir(char *file, int type)
{
	t_redir	*c_redir;
	t_list	*redir;

	c_redir = malloc(sizeof(t_redir));
	if (c_redir == NULL)
		return (NULL);
	c_redir->file = file;
	c_redir->type = type;
	redir = ft_lstnew(c_redir);
	if (redir == NULL)
		return (free(c_redir),NULL);
	redir->next = NULL;
	return (redir);
}
