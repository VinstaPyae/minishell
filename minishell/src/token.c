#include "minishell.h"

t_list	*create_token(char *str, int type)
{
	t_token	*c_token;
	t_list	*token;

	c_token = malloc(sizeof(t_token));
	if (c_token == NULL)
		return (NULL);
	c_token->token = str;
	c_token->type = type;
	token = ft_lstnew(c_token);
	if (token == NULL)
		return (free(c_token),NULL);
	token->next = NULL;
	return (token);
}