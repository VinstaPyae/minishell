/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lex_wd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 15:44:36 by pzaw              #+#    #+#             */
/*   Updated: 2025/03/30 15:44:36 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	lex_token_wd(char *str, int *i, t_list **l_token)
{
	t_list	*token;
	char	*str_token;
	int		len;
	int		space;

	len = wd_len(&str[*i]);
	space = 0;
	if (len > 0)
	{
		if (ft_isspace(str[*i + len]))
			space = 1;
		str_token = ft_substr(str, *i, len);
		if (str_token == NULL)
			return (free(str_token), 1);
		token = create_token(str_token, TOKEN_WD, space);
		if (!token)
			return (free(str_token), 1);
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
	char	*tmp;

	v_len = variable_len(&str[*i]);
	space = 0;

	if (*l_token)
		tmp = ft_strdup(token_content(*l_token)->token);
	else
		tmp = NULL;

	if (v_len > 0)
	{
		if (ft_isspace(str[*i + v_len]))
			space += 1;
		str_token = ft_substr(str, *i, v_len);
		if (!str_token)
			return (free(tmp), 1);
		if (*l_token && token_content(*l_token)->type == TOKEN_VARIABLE)
		{
			char *joined = ft_strjoin(tmp, str_token);
			free(token_content(*l_token)->token);
			token_content(*l_token)->token = joined;
			free(str_token);
		}
		else
		{
			token = create_token(str_token, TOKEN_VARIABLE, space);
			if (!token)
				return (free(str_token), free(tmp), 1);
			ft_lstadd_back(l_token, token);
		}
		(*i) += v_len;
	}
	free(tmp);
	return (0);
}

