/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_handle.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 01:55:12 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 01:55:12 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	cmd_error_msg(t_error_cmd cmd_err, char *cmd, t_minishell *shell)
{
	if (cmd_err == CMD_IS_DIR)
	{
		write(2, cmd, ft_strlen(cmd));
		write(2, ": Is a directory\n", 17);
		return (return_with_status(shell, 126));
	}
	else if (cmd_err == CMD_NO_PERM)
	{
		write(2, cmd, ft_strlen(cmd));
		write(2, ": Permission denied\n", 21);
		return (return_with_status(shell, 126));
	}
	else if (ft_strchr(cmd, '/'))
	{
		write(2, cmd, ft_strlen(cmd));
		write(2, ": No such file or directory\n", 28);
		return (return_with_status(shell, 127));
	}
	else
	{
		write(2, cmd, ft_strlen(cmd));
		write(2, ": Command not found\n", 20);
		return (return_with_status(shell, 127));
	}
}

void	free_env_list(t_env *head)
{
	t_env	*tmp;

	while (head)
	{
		tmp = head->next;
		if (head->key)
			free(head->key);
		if (head->value)
			free(head->value);
		free(head);
		head = tmp;
	}
}

void	cleanup(t_minishell **shell)
{
	if (!shell || !*shell)
		return ;
	if ((*shell)->input)
	{
		free((*shell)->input);
		(*shell)->input = NULL;
	}
	if ((*shell)->l_token)
	{
		ft_lstclear(&(*shell)->l_token, c_token_destroy);
		(*shell)->l_token = NULL;
	}
	if ((*shell)->ast)
	{
		free_ast((*shell)->ast);
		(*shell)->ast = NULL;
	}
	if ((*shell)->path)
	{
		free((*shell)->path);
		(*shell)->path = NULL;
	}
}

void	c_token_destroy(void *c_token)
{
	t_token	*token;

	token = (t_token *)c_token;
	if (!token)
		return ;
	if (token->token)
	{
		free(token->token);
		token->token = NULL;
	}
	free(token);
}

void	free_ast(t_ast_node *node)
{
	int	i;

	i = 0;
	if (!node)
		return ;
	if (node->cmd_arg)
	{
		free_arg(node->cmd_arg);
		node->cmd_arg = NULL;
	}
	if (node->redir)
	{
		ft_lstclear(&node->redir, free_redir);
		node->redir = NULL;
	}
	free_ast(node->left);
	free_ast(node->right);
	free(node);
}
