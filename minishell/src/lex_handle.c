#include "minishell.h"

int	lex_token_pipe(char *str, int *i, t_list **l_token)
{
	t_list	*token;
	char	*str_token;

	if (str[*i] == '|')
	{
		str_token = ft_substr(str, *i, 1);
		if (str_token == NULL)
			return (free(str_token),1);
		token = create_token(str_token, TOKEN_PIPE);
		if (!token)
			return (free(str_token), free_token_list(&token),1);
		ft_lstadd_back(l_token, token);
		(*i)++;
	}
	return (0);
}

/* int	lex_token_bracket(char *str, int *i, t_list **l_token)
{
	t_list	*token;
	char	*str_token;

	if (str[*i] == '(' || str[*i] == ')')
	{
		str_token = ft_substr(str, *i, 1);
		if (str_token == NULL)
			return (free(str_token), 1);
		if (str[*i] == '(')
			token = create_token(str_token, TOKEN_OBRACKET);
		else
			token = create_token(str_token, TOKEN_CBRACKET);
		if (!token)
			return (free(str_token), free_token_list(&token), 1);
		ft_lstadd_back(l_token, token);
		(*i)++;
	}
	return (0);
} */

int	lex_token_quote(char *str, int *i, t_list **l_token)
{
	t_list	*token;
	char	*str_token;
	int	q_len;

	q_len = quote_len(&str[*i]);
	if (q_len == -1)
		return (printf("Error: Unclosed quote detected\n"), 1);
	if (q_len > 0)
	{
		str_token = ft_substr(str, *i + 1, q_len - 2);
		if (str_token == NULL)
			return (free(str_token), 1);
		if (str[*i] == '"')
			token = create_token(str_token, TOKEN_DQUOTE);
		else
			token = create_token(str_token, TOKEN_SQUOTE);
		if (!token)
			return (free(str_token), free_token_list(&token), 1);
		ft_lstadd_back(l_token, token);
		(*i) += q_len;
	}
	return (0);
}

int	lex_token_redirin_hdc(char *str, int *i, t_list **l_token)
{
	t_list	*token;
	char	*str_token;
	int	*j;
	int	r;

	j = i;
	r = 1;
	if (str[*i] == '<')
	{
		if (str[*j + 1] == '<')
			r++;
		str_token = ft_substr(str, *i, r);
		if (str_token == NULL)
			return (free(str_token), 1);
		if (str[*j + 1] == '<')
			token = create_token(str_token, TOKEN_HDC);
		else
			token = create_token(str_token, TOKEN_REDIRECT_IN);
		(*i) += r;
		if (!token)
			return (free(str_token), free_token_list(&token), 1);
		ft_lstadd_back(l_token, token);
	}
	return (0);
}

int	lex_token_redirout_app(char *str, int *i, t_list **l_token)
{
	t_list	*token;
	char	*str_token;
	int	*j;
	int	r;

	j = i;
	r = 1;
	if (str[*i] == '>')
	{
		if (str[*j] == str[*j + 1])
			r++;
		str_token = ft_substr(str, *i, r);
		if (str_token == NULL)
			return (free(str_token), 1);
		if (str[*j + 1] == '>')
			token = create_token(str_token, TOKEN_APPEND);
		else
			token = create_token(str_token, TOKEN_REDIRECT_OUT);
		(*i) += r;
		if (!token)
			return (free(str_token), free_token_list(&token), 1);
		ft_lstadd_back(l_token, token);
	}
	return (0);
}
