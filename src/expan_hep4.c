/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_hep4.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 03:49:13 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 03:49:14 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	update_token_with_expansion(t_token *token,
	char *expanded_value, int space)
{
	char	*new_token;

	new_token = ft_strdup(expanded_value);
	if (!new_token)
		return ;
	free(token->token);
	token->token = new_token;
	token->type = TOKEN_WD;
	token->space = space;
}

void	handle_empty_expansion(t_token *token)
{
	char	*empty_token;

	empty_token = ft_strdup("");
	if (!empty_token)
		return ;
	free(token->token);
	token->token = empty_token;
	token->type = TOKEN_WD;
}

void	process_double_quote_token(t_token *token, t_minishell *shell)
{
	char	*d_qvalue;

	d_qvalue = expand_double_quotes(token->token, shell);
	if (!d_qvalue)
		return ;
	free(token->token);
	token->token = d_qvalue;
}

int	count_words(char *expanded_value)
{
	int	i;
	int	word_count;

	i = 0;
	word_count = 0;
	while (expanded_value[i])
	{
		while (expanded_value[i]
			&& ft_isspace((unsigned char)expanded_value[i]))
			i++;
		if (expanded_value[i] && !ft_isspace((unsigned char)expanded_value[i]))
		{
			word_count++;
			while (expanded_value[i]
				&& !ft_isspace((unsigned char)expanded_value[i]))
				i++;
		}
	}
	return (word_count);
}

char	**allocate_result_array(int word_count)
{
	char	**result;

	result = malloc(sizeof(char *) * (word_count + 1));
	if (!result)
		return (NULL);
	return (result);
}
