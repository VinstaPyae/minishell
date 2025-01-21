#include "minishell.h"

t_ast_node	*parse_pipe(t_list **tokens)
{
	t_ast_node	*left;
	t_ast_node	*p_node;

	if (!(*tokens))
		return (NULL);
	left = parse_cmd(tokens);
	if (!left)
		return (NULL);
	p_node = NULL;
	if ((*tokens) != NULL && (((t_token *)(*tokens)->content)->type == TOKEN_PIPE))
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
	
	cmd_node = create_node(NODE_COMMAND);
	while  (((*tokens) != NULL && ((t_token *)(*tokens)->content)->type != TOKEN_PIPE))
	{
		if ((*tokens) != NULL && is_word_token(((t_token *)(*tokens)->content)->type))
		{
			cmd_node->right = parse_arg(tokens);
		}
		else if ((*tokens) != NULL && is_redirection_token(((t_token *)(*tokens)->content)->type))
		{
			cmd_node->left = parse_redir(tokens);
		}
	}
	return (cmd_node);
}

char	**get_args(t_list **tokens)
{
	char	**args;
	int		i;

	i = 0;
	while ((*tokens) != NULL && is_word_token(((t_token *)(*tokens)->content)->type) && 
	((t_token *)(*tokens)->content)->type != TOKEN_PIPE)
	{
		args[i++] = ft_strdup(((t_token *)(*tokens)->content)->token);
		tokens = tokens->next;
	}
	args[i] = NULL;
	return (args);
}

t_ast_node	*parse_arg(t_list **tokens)
{
	t_ast_node	*arg;

	arg = NULL;
	while ((*tokens) != NULL && is_word_token(((t_token *)(*tokens)->content)->type) && 
	((t_token *)(*tokens)->content)->type != TOKEN_PIPE)
	{
		arg = create_node(NODE_ARG);
		if (!arg->cmd)
		{	
			arg->cmd = ft_strdup(((t_token *)(*tokens)->content)->token);
			tokens = tokens->next;
		}
		if (*tokens)
			arg->arg = get_args(tokens);
	}

}

/*
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
		cmd_node = create_node(NODE_COMMAND)**redir;;
		if (!(cmd_node->cmd))
			cmd_node->cmd = ((t_token *)(*tokens)->content)->token;
		else
			cmd_node->arg[i++] = ((t_token *)(*tokens)->content)->token;
		*tokens = (*tokens)->next;
	}
	if ((*token) != NULL && ((((t_token *)(*tokens)->content)->type == TOKEN_REDIRECT_IN) ||
	(((t_token *)(*tokens)->content)->type == TOKENint is_redirection_token(t_token_type type)_REDIRECT_OUT) || (((t_token *)(*tokens)->content)->type == TOKEN_APPEND)
	|| (((t_token *)(*tokens)->content)->type == TOKEN_HDC)))
	{
		cmd_node->redir = create_redir(NULL, ((t_token *)(*tokens)->content)->type);
		*tokens = (*tokens)->next;
		if ((*token) != NULL && ((((t_token *)(*tokens)->content)->type == TOKEN_WD) || (((t_token *)(*tokens)->content)->type == TOKEN_DQUOTE) ||
	 	(((t_token *)(*tokens)->content)->type == TOKEN_SQUOTE)))
			((t_redir *)(cmd_node)->redir)->file = ((t_token *)(*tokens)->content)->token;
	}
	return (cmd_node);**redir;
}*/

t_ast_node *create_node(t_node_type type)
{
    t_ast_node *node = malloc(sizeof(t_ast_node));
    if (!node) {
        perror("malloc"); // Print error if malloc fails
        exit(EXIT_FAILURE); // Exit the program
    }
    node->type = type;
    node->cmd = NULL;       // Command is initially NULL
    node->arg = NULL;       // Arguments list is initially NULL
    node->redir = NULL;     // Redirections list is NULL (no redirections yet)
    node->left = NULL;      // Left child is NULL
    node->right = NULL;     // Right child is NULL
    return node; // Return the newly created node
}
