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
// Helper function to duplicate the last token's content
char *duplicate_last_token(t_list **l_token)
{
    char *tmp;

    tmp = NULL;
    if (*l_token)
        tmp = ft_strdup(token_content(*l_token)->token);
    return tmp;
}

// Helper function to handle concatenation of TOKEN_VARIABLE
int handle_variable_concatenation(t_list **l_token, char *tmp, char *str_token)
{
    char *joined;

    joined = ft_strjoin(tmp, str_token);
    if (!joined)
    {
        free(str_token);
        free(tmp);
        return 1;
    }

    free(token_content(*l_token)->token);
    token_content(*l_token)->token = joined;
    free(str_token);
    return 0;
}

// Helper function to create a new TOKEN_VARIABLE
int create_variable_token(t_list **l_token, char *str_token, int space)
{
    t_list *token;

    token = create_token(str_token, TOKEN_VARIABLE, space);
    if (!token)
    {
        free(str_token);
        return 1;
    }

    ft_lstadd_back(l_token, token);
    return 0;
}

// Main function to handle lexing of variable tokens
// Helper function to calculate variable length and initialize space
void calculate_variable_length(char *str, int *i, int *v_len, int *space)
{
    *v_len = variable_len(&str[*i]);
    *space = 0;

    if (*v_len > 0 && ft_isspace(str[*i + *v_len]))
        *space = 1;
}

// Helper function to extract variable token
char *extract_variable_token(char *str, int *i, int v_len)
{
    char *str_token;

    str_token = ft_substr(str, *i, v_len);
    if (!str_token)
        return NULL;

    return str_token;
}

// Helper function to process variable token
int process_variable_token(t_list **l_token, char *tmp, char *str_token, int space)
{
    if (*l_token && token_content(*l_token)->type == TOKEN_VARIABLE)
    {
        if (handle_variable_concatenation(l_token, tmp, str_token))
            return 1;
    }
    else
    {
        if (create_variable_token(l_token, str_token, space))
            return 1;
    }
    return 0;
}

// Main function to handle lexing of variable tokens
int lex_token_variable(char *str, int *i, t_list **l_token)
{
    int v_len;
    int space;
    char *tmp;
    char *str_token;

    tmp = duplicate_last_token(l_token);
    calculate_variable_length(str, i, &v_len, &space);
    if (v_len > 0)
    {
        str_token = extract_variable_token(str, i, v_len);
        if (!str_token)
        {
            free(tmp);
            return 1;
        }
        if (process_variable_token(l_token, tmp, str_token, space))
        {
            free(tmp);
            return 1;
        }
        *i += v_len;
    }
    free(tmp);
    return 0;
}
