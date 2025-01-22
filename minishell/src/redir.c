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

int is_word_token(t_token_type type)
{
    return (type == TOKEN_WD || type == TOKEN_SQUOTE || type == TOKEN_DQUOTE || type == TOKEN_VARIABLE);
}

int is_redirection_token(t_token_type type)
{
    return (type == TOKEN_REDIRECT_IN || type == TOKEN_REDIRECT_OUT ||
            type == TOKEN_APPEND || type == TOKEN_HDC);
}