#include "minishell.h"

void	cleanup(t_list **l_token, char **input, t_ast_node **ast)
{
	if (*input)
	{
		free(*input);
		*input = NULL;
	}
	if (*l_token)
	{
		ft_lstclear(l_token, c_token_destroy);
		*l_token = NULL;
	}
	if (*ast)
	{
		free_ast(*ast); // Free the AST tree
		*ast = NULL;
	}
	rl_clear_history();
}

void free_ast(t_ast_node *node)
{
	int	i;

	i = 0;
	if (!node)
		return;

	// Free command arguments
	if (node->cmd_arg)
	{
		while (node->cmd_arg[i])
		{
			free(node->cmd_arg[i]); // Free each string in the array
			node->cmd_arg[i++] = NULL; // Avoid dangling pointers
		}
		free(node->cmd_arg); // Free the array itself
		node->cmd_arg = NULL; // Avoid dangling pointers
	}

	// Free redirections
	if (node->redir)
	{
		ft_lstclear(&node->redir, free); // Free the redirection list
		node->redir = NULL; // Avoid dangling pointers
	}

	// Recursively free left and right nodes
	free_ast(node->left);
	free_ast(node->right);

	// Free the node itself
	free(node);
}

/*void	cleanup(t_list **tokens, char **input, t_ast_node **ast)
{
	if (input && *input)
	{
		free(*input);
		*input = NULL;
	}
	if (tokens && *tokens)
	{
		ft_lstclear(tokens, c_token_destroy);  // Assuming free_token frees a token
		*tokens = NULL;
	}
	if (ast && *ast)
	{
		free_ast(*ast);  // Assuming free_ast frees the AST
		*ast = NULL;
	}
}*/