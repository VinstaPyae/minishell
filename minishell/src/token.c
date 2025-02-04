#include "minishell.h"

t_list	*create_token(char *str, int type, int s)
{
	t_token	*c_token;
	t_list	*token;

	c_token = malloc(sizeof(t_token));
	if (c_token == NULL)
		return (NULL);
	c_token->token = str;
	c_token->type = type;
	c_token->space = s;
	token = ft_lstnew(c_token);
	if (token == NULL)
		return (free(c_token),NULL);
	token->next = NULL;
	return (token);
}

t_token	*token_content(t_list *token)
{
	return ((t_token *)token->content);
}


void	printer_token(t_list *l_token)
{
	if (l_token != NULL)
	{
		while (l_token->next != NULL)
		{
			printf("Token: Type = %d, Value = %s \n", token_content(l_token)->type, token_content(l_token)->token);
			l_token = l_token->next;
		}
		printf("Token: Type = %d, Value = %s \n", token_content(l_token)->type, token_content(l_token)->token);
	}
}
