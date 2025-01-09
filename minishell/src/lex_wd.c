#include "minishell.h"

int	ot_len(char *input)
{
	if (*input == '\'' || *input == '"' || *input == '(' || *input == ')'
	|| *input == '|' || *input == '$')
		return (1);
	if ((*input == '>' && *(input + 1) != '>') || (*input == '<' && *(input + 1) != '<'))
		return (1);
	if ((*input == '>' && *(input + 1) == '>') || (*input == '<' && *(input + 1) == '<'))
		return (2);
	return (0);
}

int	wd_len(char *input)
{
	int	i;

	i = 0;
	while (input[i])
	{
		if (input[i] == 32 || (input[i] >= 9 && input[i] <= 13))
			break;
		if (ot_len(&input[i]) > 0)
			break;
		i++;
	}
	return (i);
}

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
			return (free(str_token), 1);
		token = create_token(str_token, TOKEN_WD);
		if (!token)
			return (free(str_token),free(token), 1);
		ft_lstadd_back(l_token, token);
		(*i) += len;
	}
	return (0);
}

int	lex_token_variable(char *str, int *i, t_list **l_token)
{
	t_list	*token;
	char	*str_token;

	if (str[*i] == '$')
	{
		str_token = ft_substr(str, *i, 1);
		if (!str_token)
			return (free(str_token), 1);
		token = create_token(str_token, TOKEN_VARIABLE);
		if (!token)
			return (free(token),free(str_token),1);
		ft_lstadd_back(l_token, token);
		(*i)++;
	}
	return (0);
}

void	free_token_list(t_list **l_token)
{
	t_list *temp;
	t_token *content;

	while (*l_token)
	{
		temp = *l_token;
		content = (t_token *)temp->content;
		if (content)
		{
			free(content->token); // Free the token string
			free(content);        // Free the token struct
		}
		*l_token = temp->next;
		free(temp); // Free the list node
	}
	*l_token = NULL;
}