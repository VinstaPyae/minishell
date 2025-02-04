#include "minishell.h"

int	lex_token_pipe(char *str, int *i, t_list **l_token)
{
	t_list	*token;
	char	*str_token;

	if (str[*i] == '|')
	{
		str_token = ft_substr(str, *i, 1);
		if (str_token == NULL)
			return (print_error(__func__, __FILE__, __LINE__, "Mem failed for str_token"), free(str_token),1);
		token = create_token(str_token, TOKEN_PIPE, 0);
		if (!token)
			return (print_error(__func__, __FILE__, __LINE__, "Failed to create token for word"), free(str_token), ft_lstclear(&token, c_token_destroy), 1);
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
	int	c_space;

	q_len = quote_len(&str[*i]);
	c_space = 0;
	if (q_len == -1)
		return (printf("Error: Unclosed quote detected\n"), 1);
	if (q_len > 0)
	{
		if (ft_isspace(str[*i + q_len]))
			c_space += 1;
		str_token = ft_substr(str, *i + 1, q_len - 2);
		if (str_token == NULL)
			return (print_error(__func__, __FILE__, __LINE__, "Mem failed for str_token"), free(str_token), 1);
		if (str[*i] == '"')
			token = create_token(str_token, TOKEN_DQUOTE, c_space);
		else
			token = create_token(str_token, TOKEN_SQUOTE, c_space);
		if (!token)
		{
			print_error(__func__, __FILE__, __LINE__, "Failed to create token for quote");
			return (free(str_token), ft_lstclear(&token, c_token_destroy), 1);
		}
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
			return (print_error(__func__, __FILE__, __LINE__, "Mem allocate failed for str_token"), free(str_token), 1);
		if (str[*j + 1] == '<')
			token = create_token(str_token, TOKEN_HDC, 0);
		else
			token = create_token(str_token, TOKEN_REDIRECT_IN, 0);
		(*i) += r;
		if (!token)
			return (print_error(__func__, __FILE__, __LINE__, "Failed to create token for word"), free(str_token), ft_lstclear(&token, c_token_destroy), 1);
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
			return (print_error(__func__, __FILE__, __LINE__, "Mem failed for str_token"), free(str_token), 1);
		if (str[*j + 1] == '>')
			token = create_token(str_token, TOKEN_APPEND, 0);
		else
			token = create_token(str_token, TOKEN_REDIRECT_OUT, 0);
		(*i) += r;
		if (!token)
			return (print_error(__func__, __FILE__, __LINE__, "Failed to create token for word"), free(str_token), ft_lstclear(&token, c_token_destroy), 1);
		ft_lstadd_back(l_token, token);
	}
	return (0);
}
