#include "minishell.h"

int	lex_token_wd(char *str, int *i, t_list **l_token)
{
	t_list	*token;
	char	*str_token;
	int	len;

	len = wd_len(&str[*i]);
	if (len > 0)
	{
		str_token = ft_substr(str, *i, len);
		if (str_token == NULL)
		{
			print_error(__func__, __FILE__, __LINE__, "Mem allocate failed for str_token");
			return (free(str_token), 1);
		}
		token = create_token(str_token, TOKEN_WD);
		if (!token)
		{
			print_error(__func__, __FILE__, __LINE__, "Failed to create token for word");
			return (free(str_token), ft_lstclear(&token, c_token_destroy), 1);
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

	v_len = variable_len(&str[*i]);
	if (v_len > 0)
	{
		str_token = ft_substr(str, *i, v_len);
		if (!str_token)
			return (free(str_token), 1);
		token = create_token(str_token, TOKEN_VARIABLE);
		if (!token)
			return (free(str_token), ft_lstclear(&token, c_token_destroy), 1);
		ft_lstadd_back(l_token, token);
		(*i) += v_len;
	}
	return (0);
}
