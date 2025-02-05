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
    if (!shell || !*shell)
        return;

    // Free input
    if ((*shell)->input)
    {
        free((*shell)->input);
        (*shell)->input = NULL;
    }

    // Free tokens
    if ((*shell)->l_token)
    {
        ft_lstclear(&(*shell)->l_token, c_token_destroy);
        (*shell)->l_token = NULL;
    }

    // Free AST
    if ((*shell)->ast)
    {
        free_ast((*shell)->ast);
        (*shell)->ast = NULL;
    }
	free(*shell);
    *shell = NULL;
    // Do not free envp here, as it is shared across iterations
}
void c_token_destroy(void *c_token)
{
	t_token *token = (t_token *)c_token;
	if (!token)
		return;
	
	if (token->token)
	{
		free(token->token);
		token->token = NULL; // Avoid dangling pointer
	}

	free(token);
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
	}
	if (node->cmd_arg)
	{
		free_arg(node->cmd_arg);
	}

	// Free redirections
	if (node->redir)
	{
		ft_lstclear(&node->redir, free_redir); // Free the redirection list
		node->redir = NULL; // Avoid dangling pointers
	}

	// Recursively free left and right nodes
	free_ast(node->left);
	free_ast(node->right);

	// Free the node itself
	free(node);
}

void free_redir(void *redir)
{
	t_redir *r = (t_redir *)redir;
	if (r)
	{
		if (r->file)
			free(r->file);  // Free filename if dynamically allocated
		free(r);
	}
}


void	free_arg(char **str)
{
	int	i;

	if (!str) // Check if str is NULL before accessing it
		return;
	
	i = 0;
	while (str[i])
	{
		free(str[i]);
		i++;
	}
	free(str);
}

void	remove_node(t_list **head, t_list *node_to_remove, void (*del)(void *))
{
    t_list *current = *head;
    t_list *prev = NULL;

    while (current)
    {
        if (current == node_to_remove)
        {
            if (prev)
                prev->next = current->next;
            else
                *head = current->next;
            ft_lstdelone(current, del);
            return;
        }
        prev = current;
        current = current->next;
    }
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
