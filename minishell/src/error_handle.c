#include "minishell.h"

/* void cleanup(t_minishell **shell)
{
    if (!shell || !*shell) // Check if shell or *shell is NULL
        return;

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
        free_ast((*shell)->ast); // Free the AST tree
        (*shell)->ast = NULL;
    }

    free(*shell); // Free the shell structure
    *shell = NULL; // Set the pointer to NULL to avoid dangling references
} */

void cleanup(t_minishell **shell)
{
    if (!shell)
    {
        printf("cleanup: shell is NULL\n");
        return;
    }
    if (!*shell)
    {
        printf("cleanup: *shell is NULL\n");
        return;
    }

    printf("cleanup: cleaning up shell structure at address %p\n", (void*)*shell);

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

    printf("cleanup: freeing shell at address %p\n", (void*)*shell);
    free(*shell);
    *shell = NULL;
    printf("cleanup: shell set to NULL\n");
}

void free_ast(t_ast_node *node)
{
	int	i;

	i = 0;
	if (!node)
		return;

	// Free command arguments
	if (node->cmd)
	{
		free(node->cmd);
		node->cmd = NULL;
	}
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