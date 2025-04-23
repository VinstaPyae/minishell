/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 15:07:41 by pzaw              #+#    #+#             */
/*   Updated: 2025/03/30 15:07:41 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_list	*get_token_list(char *input)
{
	t_list	*l_token;
	int		i;
	int		space;

	i = 0;
	space = 0;
	l_token = NULL;
	while (input[i])
	{
		if (lex_token_pipe(input, &i, &l_token)
			|| lex_token_quote(input, &i, &l_token, space)
			|| lex_token_redirin_hdc(input, &i, &l_token)
			|| lex_token_redirout_app(input, &i, &l_token)
			|| lex_token_variable(input, &i, &l_token)
			|| lex_token_wd(input, &i, &l_token))
			break ;
		while (input[i] == 32 || (input[i] >= 9 && input[i] <= 13))
			i++;
	}
	if (input[i] != '\0')
		return (ft_lstclear(&l_token, c_token_destroy), NULL);
	return (l_token);
}

int	lexer_syntax_check(t_list *l_token)
{
	t_token	*next;

	next = NULL;
	if (check_pipe_grammar(l_token) == 1)
		return (1);
	if (check_redirect_in_grammar(l_token) == 1)
		return (1);
	if (check_redirect_out_grammar(l_token) == 1)
		return (1);
	if (check_append_grammar(l_token) == 1)
		return (1);
	if (check_heredoc_grammar(l_token, next) == 1)
		return (1);
	return (0);
}

t_list	*lexer(t_minishell *shell)
{
	t_list	*l_token;

	l_token = get_token_list(shell->input);
	free(shell->input);
	shell->input = NULL;
	if (!l_token)
		return (ft_lstclear(&l_token, c_token_destroy), NULL);
	if (lexer_syntax_check(l_token) == 1)
		return (ft_lstclear(&l_token, c_token_destroy), NULL);
	return (l_token);
}
