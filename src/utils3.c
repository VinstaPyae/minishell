/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils3.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 02:05:45 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 02:05:49 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_isspace(int c)
{
	if (c == ' ' || c == '\n' || c == '\t' || c == '\v' || c == '\f'
		|| c == '\r')
		return (1);
	return (0);
}

int	variable_len(char	*input)
{
	int	i;

	if (!input || input[0] != '$')
		return (0);
	i = 1;
	if ((input[i] >= '0' && input[i] <= '9') || input[i] == '?')
		return (i + 1);
	while (input[i] && (ft_isalnum(input[i]) || ft_isalpha(input[i])
			|| input[i] == '_' || input[i] == '$'))
	{
		if (ft_isspace(input[i]) || (input[i] == '\'' || input[i] == '"')
			|| (input[i] == '$' && (input[i + 1] == '\0'
					|| ft_isspace(input[i + 1]) || input[i + 1] == '$')))
			break ;
		i++;
	}
	return (i);
}

int	ot_len(char *input)
{
	if (*input == '\'' || *input == '"' || *input == '|' || *input == '$')
		return (1);
	if ((*input == '>' && *(input + 1) != '>')
		|| (*input == '<' && *(input + 1) != '<'))
		return (1);
	if ((*input == '>' && *(input + 1) == '>')
		|| (*input == '<' && *(input + 1) == '<'))
		return (2);
	return (0);
}

char	**trim_cmd(char	**cmd_arg)
{
	char	**cmd;
	int		i;

	i = 0;
	if (!cmd_arg || !cmd_arg[0])
		return (NULL);
	while (cmd_arg[i])
		i++;
	cmd = malloc((i + 1) * sizeof(char *));
	if (!cmd)
		return (NULL);
	i = 0;
	while (cmd_arg[i])
	{
		cmd[i] = ft_strtrim(cmd_arg[i], " ");
		if (!cmd[i])
			free_arg(cmd);
		i++;
	}
	cmd[i] = NULL;
	return (cmd);
}

char	*trim_last_char(const char *s, char c)
{
	size_t	len;
	char	*trimmed;

	if (!s)
		return (NULL);
	len = ft_strlen(s);
	if (len > 0 && s[len - 1] == c)
		len--;
	trimmed = (char *)malloc(len + 1);
	if (!trimmed)
		return (NULL);
	ft_memcpy(trimmed, s, len);
	trimmed[len] = '\0';
	return (trimmed);
}
