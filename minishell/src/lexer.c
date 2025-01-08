#include "minishell.h"

t_list *get_token_list(char *input)
{
	t_list *l_token;
	int	i;

	i = 0;
	while (input[i])
	{
		if (lex_token_pipe(input, &i, &l_token) == -1)
			break;
		else
			i++;		
	}
	return (l_token);
}

t_list	*lexer(char *input)
{
	t_list	*l_token;

	l_token = get_token_list(input);
	if (!l_token)
		return (NULL);
	return (l_token);
}