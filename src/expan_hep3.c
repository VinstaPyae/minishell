/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_hep3.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 03:49:09 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 03:49:10 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*expand_quote_variable(char *var, t_minishell *shell)
{
	char	*value;
	char	*dup;

	if (!var)
		return (NULL);
	if (var[1] == '\0')
		return (ft_strdup("$"));
	if (var[1] == '?' && var[2] == '\0')
		return (ft_itoa(shell->exit_status));
	value = get_env_value(shell->envp, &var[1]);
	if (value)
	{
		dup = ft_strdup(value);
		free(value);
		return (dup);
	}
	return (ft_strdup(""));
}

char	*extract_variable_name(const char *input, int *i)
{
	char	*var_name;
	int		j;

	j = *i + 1;
	if (input[j] == '?')
		j++;
	while (ft_isalnum(input[j]) || input[j] == '_')
		j++;
	var_name = ft_substr(input, *i, j - *i);
	*i = j;
	return (var_name);
}

char	*append_expanded_variable(char *result,
	char *var_name, t_minishell *shell)
{
	char	*expanded;
	char	*temp;

	expanded = expand_quote_variable(var_name, shell);
	free(var_name);
	temp = ft_strjoin(result, expanded);
	free(result);
	free(expanded);
	return (temp);
}

char	*append_normal_character(char *result, char c)
{
	char	char_str[2];
	char	*temp;

	char_str[0] = c;
	char_str[1] = '\0';
	temp = ft_strjoin(result, char_str);
	free(result);
	return (temp);
}

char	*expand_double_quotes(char *input, t_minishell *shell)
{
	char	*result;
	char	*var_name;
	int		i;

	result = ft_strdup("");
	i = 0;
	while (input[i])
	{
		if (input[i] == '$')
		{
			var_name = extract_variable_name(input, &i);
			result = append_expanded_variable(result, var_name, shell);
		}
		else
		{
			result = append_normal_character(result, input[i]);
			i++;
		}
	}
	return (result);
}
