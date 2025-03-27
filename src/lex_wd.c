#include "minishell.h"

int	lex_token_wd(char *str, int *i, t_list **l_token)
{
	t_list	*token;
	char	*str_token;
	int	len;
	int	space;

	len = wd_len(&str[*i]);
	space = 0;
	if (len > 0)
	{
		if (ft_isspace(str[*i + len]))
			space = 1;
		str_token = ft_substr(str, *i, len);
		if (str_token == NULL)
		{
			print_error(__func__, __FILE__, __LINE__, "Mem allocate failed for str_token");
			free(str_token);
			return (1);
		}
		token = create_token(str_token, TOKEN_WD, space);
		if (!token)
		{
			print_error(__func__, __FILE__, __LINE__, "Failed to create token for word");
			return (free(str_token), 1);
		}
		ft_lstadd_back(l_token, token);
		(*i) += len;
	}
	return (0);
}

int	lex_token_variable(char *str, int *i, t_list **l_token)
{
	t_list	*token;
	char	*str_token;
	int		v_len;
	int		space;

	v_len = variable_len(&str[*i]);
	// printf("$len: %d\n\n", v_len);
	space = 0;
	if (v_len > 0)
	{
		if (ft_isspace(str[*i + v_len]))
			space += 1;
		str_token = ft_substr(str, *i, v_len);
		if (!str_token)
			return (free(str_token), 1);
		token = create_token(str_token, TOKEN_VARIABLE, space);
		if (!token)
			return (free(str_token), ft_lstclear(&token, c_token_destroy), 1);
		ft_lstadd_back(l_token, token);
		(*i) += v_len;
	}
	return (0);
}
