/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_hep5.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 03:49:17 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 03:49:18 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*extract_word(char *expanded_value, int start, int len)
{
	char	*word;

	word = ft_substr(expanded_value, start, len);
	return (word);
}

int	populate_result_array(char **result,
	char *ev, int word_count)
{
	int	i;
	int	idx;
	int	start;
	int	len;

	i = 0;
	idx = -1;
	while (ev[i] && ++idx < word_count)
	{
		while (ev[i] && ft_isspace((unsigned char)ev[i]))
			i++;
		start = i;
		while (ev[i] && !ft_isspace((unsigned char)ev[i]))
			i++;
		len = i - start;
		result[idx] = extract_word(ev, start, len);
		if (!result[idx])
		{
			while (idx-- > 0)
				free(result[idx]);
			return (free(result), 0);
		}
	}
	result[word_count] = NULL;
	return (1);
}

char	**split_expanded_value(char *expanded_value)
{
	char	**result;
	int		word_count;

	result = NULL;
	word_count = count_words(expanded_value);
	result = allocate_result_array(word_count);
	if (!result)
		return (NULL);
	if (!populate_result_array(result, expanded_value, word_count))
		return (NULL);
	return (result);
}

char	*join_expanded_values(char **expanded_value)
{
	char	*result;
	char	*tmp;
	int		i;

	result = NULL;
	i = 0;
	while (expanded_value[i])
	{
		if (!result)
		{
			result = ft_strdup(expanded_value[i]);
		}
		else
		{
			tmp = result;
			result = ft_strjoin(tmp, expanded_value[i]);
			free(tmp);
		}
		i++;
	}
	return (result);
}

char	**handle_split_value(char *result, char **split_value)
{
	if (split_value)
	{
		if (!split_value[0])
		{
			free_arg(split_value);
			split_value = create_single_result(ft_strdup(""));
		}
		else if (!split_value[1])
		{
			free_arg(split_value);
			split_value = create_single_result(ft_strdup(result));
		}
	}
	return (split_value);
}
