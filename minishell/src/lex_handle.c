#include "minishell.h"

int	lex_token_pipe(char *str, int *i, t_list **l_token)
{
	t_list	*token;
	char	*str_token;

	if (str[*i] == '|')
	{
		str_token = ft_substr(str, *i, 1);
		if (str_token == NULL)
			return (1);
		token = create_token(str_token, TOKEN_PIPE);
		if (!token)
		{
			free(token);
			return (1);
		}
		ft_lstadd_back(l_token, token);
		(*i)++;
	}
	return (0);
}