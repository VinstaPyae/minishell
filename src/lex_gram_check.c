/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lex_gram_check.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 15:51:22 by pzaw              #+#    #+#             */
/*   Updated: 2025/03/30 15:51:22 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	check_pipe_grammar(t_list *l_token)
{
	t_token	*current;
	t_token	*next;

	if (!l_token)
		return (1);
	current = l_token->content;
	if (current->type == TOKEN_PIPE)
		return (printf("Syntax error: Pipe '|' cannot appear at the start"
				" of the input\n"), 1);
	while (l_token)
	{
		current = l_token->content;
		next = NULL;
		if (l_token->next)
			next = l_token->next->content;
		if (current->type == TOKEN_PIPE)
		{
			if (!next)
				return (printf("Syntax error: Pipe '|' cannot appear"
						" at the end of the input\n"), 1);
		}
		l_token = l_token->next;
	}
	return (0);
}

int	check_redirect_in_grammar(t_list *l_token)
{
	t_token	*current;
	t_token	*next;

	if (!l_token)
		return (1);
	while (l_token)
	{
		current = l_token->content;
		next = NULL;
		if (l_token->next)
			next = l_token->next->content;
		if (current->type == TOKEN_REDIRECT_IN)
		{
			if (!next)
				return (printf("Syntax error: Input redirection '<' cannot"
						" appear at the end of the input\n"), 1);
			if (next->type != TOKEN_WD && next->type != TOKEN_DQUOTE
				&& next->type != TOKEN_SQUOTE)
				return (printf("Syntax error: Invalid token '%s' "
						"after input redirection '<'\n", next->token), 1);
		}
		l_token = l_token->next;
	}
	return (0);
}

int	check_redirect_out_grammar(t_list *l_token)
{
	t_token	*current;
	t_token	*next;

	if (!l_token)
		return (1);
	while (l_token)
	{
		current = l_token->content;
		next = NULL;
		if (l_token->next)
			next = l_token->next->content;
		if (current->type == TOKEN_REDIRECT_OUT)
		{
			if (!next)
				return (printf("Syntax error: Output redirection '>' "
						"cannot appear at the end of the input\n"), 1);
			if (next->type != TOKEN_WD && next->type != TOKEN_DQUOTE
				&& next->type != TOKEN_SQUOTE)
				return (printf("Syntax error: Invalid token '%s' "
						"after output redirection '<'\n", next->token), 1);
		}
		l_token = l_token->next;
	}
	return (0);
}

int	check_append_grammar(t_list *l_token)
{
	t_token	*current;
	t_token	*next;

	if (!l_token)
		return (1);
	while (l_token)
	{
		current = l_token->content;
		next = NULL;
		if (l_token->next)
			next = l_token->next->content;
		if (current->type == TOKEN_APPEND)
		{
			if (!next)
				return (printf("Syntax error: Append '>>' cannot appear"
						" at the end of the input\n"), 1);
			if (next->type != TOKEN_WD && next->type != TOKEN_DQUOTE
				&& next->type != TOKEN_SQUOTE && next->type != TOKEN_VARIABLE)
				return (printf("Syntax error: Invalid token '%s' "
						"after Append '>>'\n", next->token), 1);
		}
		l_token = l_token->next;
	}
	return (0);
}

int	check_heredoc_grammar(t_list *l_token, t_token *next)
{
	t_token	*current;

	if (!l_token)
		return (1);
	while (l_token)
	{
		current = l_token->content;
		if (l_token->next)
			next = l_token->next->content;
		if (current->type == TOKEN_HDC)
		{
			if (!next)
				return (printf("Syntax error: Heredoc '<<' cannot appear "
						"at the end of the input\n"), 1);
			if (next->type != TOKEN_WD && next->type != TOKEN_DQUOTE
				&& next->type != TOKEN_SQUOTE && next->type != TOKEN_VARIABLE)
				return (printf("Syntax error: Invalid token '%s' "
						"after Heredoc '<<'\n", next->token), 1);
			if (next->type == TOKEN_VARIABLE || next->type == TOKEN_DQUOTE)
				next->type = TOKEN_WD;
		}
		l_token = l_token->next;
	}
	return (0);
}
