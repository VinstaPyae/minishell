#include "minishell.h"

t_ast_node	*parse_pipe(t_list **tokens)
{
	t_ast_node	*left;
	t_ast_node	*p_node;

	left = parse_cmd(tokens);
	node = NULL;
	if ((*token) != NULL && (((t_token *)(*tokens)->content)->type == TOKEN_PIPE))
	{
		*tokens = (*tokens)->next;
		p_node = create_node(NODE_PIPE);
		p_node->left = left;
		p_node->right = parse_pipe(tokens);
	}
	else
		return (left);
	return (p_node);
}

t_ast_node	*parse_cmd(t_list **tokens)
{
	t_ast_node	*cmd_node;
	int		i;

	if ((*tokens) == NULL)
		return (NULL);
	i = 0;
	while ((*token) != NULL && ((((t_token *)(*tokens)->content)->type == TOKEN_WD) || (((t_token *)(*tokens)->content)->type == TOKEN_DQUOTE) ||
	 (((t_token *)(*tokens)->content)->type == TOKEN_SQUOTE)))
	{
		cmd_node = create_node(NODE_COMMAND);
		if (!(cmd_node->cmd))
			cmd_node->cmd = ((t_token *)(*tokens)->content)->token;
		else
			cmd_node->arg[i++] = ((t_token *)(*tokens)->content)->token;
		*tokens = (*tokens)->next;
	}
	if ((*token) != NULL && ((((t_token *)(*tokens)->content)->type == TOKEN_REDIRECT_IN) ||
	(((t_token *)(*tokens)->content)->type == TOKEN_REDIRECT_OUT) || (((t_token *)(*tokens)->content)->type == TOKEN_APPEND)
	|| (((t_token *)(*tokens)->content)->type == TOKEN_HDC)))
	{
		cmd_node->redir = create_redir(NULL, ((t_token *)(*tokens)->content)->type);
		*tokens = (*tokens)->next;
		if ((*token) != NULL && ((((t_token *)(*tokens)->content)->type == TOKEN_WD) || (((t_token *)(*tokens)->content)->type == TOKEN_DQUOTE) ||
	 	(((t_token *)(*tokens)->content)->type == TOKEN_SQUOTE)))
			((t_redir *)(cmd_node)->redir)->file = ((t_token *)(*tokens)->content)->token;
	}
	return (cmd_node);
}

t_ast_node *create_node(t_node_type type)
{
    t_ast_node *node = malloc(sizeof(t_ast_node));
    if (!node) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    node->redir = NULL;
    node->cmd = NULL;
    node->arg = NULL;
    node->left = NULL;
    node->right = NULL;
    return node;
}
