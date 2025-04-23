/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 02:08:53 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 02:08:53 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	wd_len(char *input)
{
	int	i;

	i = 0;
	while (input[i])
	{
		if (input[i] == 32 || (input[i] >= 9 && input[i] <= 13))
			break ;
		if (ot_len(&input[i]) > 0)
			break ;
		i++;
	}
	return (i);
}

int	is_word_token(t_token_type type)
{
	return (type == TOKEN_WD || type == TOKEN_SQUOTE
		|| type == TOKEN_DQUOTE || type == TOKEN_VARIABLE);
}

int	is_redirection_token(t_token_type type)
{
	return (type == TOKEN_REDIRECT_IN || type == TOKEN_REDIRECT_OUT
		|| type == TOKEN_APPEND || type == TOKEN_HDC);
}

int	quote_len(char *str)
{
	char	quote_char;
	int		i;

	i = 0;
	if (str[i] == '\'' || str[i] == '"')
	{
		quote_char = str[i];
		i++;
		while (str[i] && str[i] != quote_char)
			i++;
		if (str[i] == quote_char)
			return (i + 1);
		else
			return (-1);
	}
	return (0);
}

int	ft_strcmp(const char *s1, const char *s2)
{
	if (!s1)
		return (-1);
	if (!s2)
		return (1);
	while (*s1 && *s2 && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return ((unsigned char)*s1 - (unsigned char)*s2);
}
