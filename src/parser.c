/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 03:05:09 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 03:05:09 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	process_arguments(t_list **tmp_list,
		char **cmd_arg, char *tmp, t_list *tokens)
{
	char	*arg;
	int		i;

	i = 0;
	while (*tmp_list != NULL && is_word_token(token_content(*tmp_list)->type)
		&& token_content(*tmp_list)->type != TOKEN_PIPE)
	{
		arg = get_argument(tmp_list, tmp, tokens);
		if (!arg)
		{
			free_arg(cmd_arg);
			free(tmp);
			return (0);
		}
		cmd_arg[i] = arg;
		i++;
		*tmp_list = (*tmp_list)->next;
	}
	cmd_arg[i] = NULL;
	return (1);
}

char	**get_cmd_args(t_list **tokens)
{
	t_list	*tmp_list;
	char	**cmd_arg;
	char	*tmp;
	int		arg_count;

	tmp = ft_strdup("");
	if (!tmp)
		return (NULL);
	if (!tokens || !(*tokens))
	{
		free(tmp);
		return (NULL);
	}
	arg_count = count_parse_arguments(*tokens);
	if (!allocate_cmd_args(&cmd_arg, arg_count, &tmp))
		return (NULL);
	tmp_list = *tokens;
	if (!process_arguments(&tmp_list, cmd_arg, tmp, *tokens))
		return (NULL);
	free(tmp);
	*tokens = tmp_list;
	return (cmd_arg);
}

t_ast_node	*create_node(t_node_type type)
{
	t_ast_node	*node;

	node = malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = type;
	node->cmd_arg = NULL;
	node->redir = NULL;
	node->left = NULL;
	node->right = NULL;
	return (node);
}

t_ast_node	*parse_pipe(t_list *tokens)
{
	t_ast_node	*left;

	if (!tokens)
		return (NULL);
	left = parse_cmd(&tokens);
	if (!left)
		return (NULL);
	if (is_pipe_token(tokens))
		return (create_pipe_node(tokens, left));
	return (left);
}

t_ast_node	*parse_cmd(t_list **tokens)
{
	t_ast_node	*cmd_node;

	cmd_node = create_command_node();
	if (!cmd_node)
		return (NULL);
	while ((*tokens) != NULL && !is_pipe_token((*tokens)))
	{
		if (is_word_token(token_content(*tokens)->type))
		{
			if (!process_command_arguments(cmd_node, tokens))
			{
				free_ast(cmd_node);
				return (NULL);
			}
		}
		else if (is_redirection_token(token_content(*tokens)->type))
		{
			if (!process_redirections(cmd_node, tokens))
			{
				free_ast(cmd_node);
				return (NULL);
			}
		}
	}
	return (cmd_node);
}
