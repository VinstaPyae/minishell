#include "minishell.h"

t_list *get_token_list(char *input)
{
	t_list *l_token;
	int	i;

	i = 0;
	l_token = NULL;
	while (input[i])
	{
		if (lex_token_pipe(input, &i, &l_token))
			break;
		/* if (lex_token_bracket(input, &i, &l_token))
			break; */
		if (lex_token_quote(input, &i, &l_token))
			break;
		if (lex_token_redirin_hdc(input, &i, &l_token))
			break;
		if (lex_token_redirout_app(input, &i, &l_token))
			break;
		if (lex_token_variable(input, &i, &l_token))
			break ;
		if (lex_token_wd(input, &i, &l_token))
			break;
		while (input[i] == 32 || (input[i] >= 9 && input[i] <= 13))
			i++;
	}
	if (input[i] != '\0')
		return (ft_lstclear(&l_token, c_token_destroy), NULL);
	return (l_token);
}

t_list	*lexer(char *input)
{
	t_list	*l_token;

	l_token = get_token_list(input);
	if (!l_token)
		return (ft_lstclear(&l_token, c_token_destroy), NULL);
	if (lexer_syntax_check(l_token) == 1)
		return (ft_lstclear(&l_token, c_token_destroy),NULL);
	return (l_token);
}
