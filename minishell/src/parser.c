#include "minishell.h"

t_ast_node *parse(t_list **tokens)
{
	t_ast_node *node = NULL;

	node = parse_pipe(tokens);      // Try parsing for pipes first
	/*if (!node)
		node = parse_redirection(tokens);  // Then check for redirections
	if (!node)
		node = parse_heredoc(tokens);*/   // Then check for heredocs
	if (!node)
		node = parse_command(tokens);  // Finally, check for commands or words

	// If more tokens remain after parsing, it's an error
	if (*tokens)
	{
		free_ast(node);  // Free the partially constructed AST
		return NULL;
	}

	return node;
}

t_ast_node *create_ast_node(t_node_type type, char *data)
{
    t_ast_node *node = (t_ast_node *)malloc(sizeof(t_ast_node));
    if (!node)
        return NULL;
    node->type = type;
    node->data = data ? strdup(data) : NULL;
    if (data && !node->data)  // Check if strdup failed
    {
        free(node);
        return NULL;
    }
    node->left = NULL;
    node->right = NULL;
    return node;
}

void free_ast(t_ast_node *node)
{
    if (node)
    {
        free(node->data);
        free_ast(node->left);
        free_ast(node->right);
        free(node);
    }
}

t_ast_node *parse_command(t_list **tokens)
{
    if (!tokens || !*tokens)
        return NULL;

    t_token *current = (t_token *)(*tokens)->content;  // Get the token content
    if (current->type == TOKEN_WD)  // If it's a word, create a command node
    {
        t_ast_node *command_node = create_ast_node(NODE_COMMAND, current->token);
        *tokens = (*tokens)->next;  // Move to the next token
        return command_node;
    }
    return NULL;
}

t_ast_node *parse_pipe(t_list **tokens)
{
    if (!tokens || !*tokens)
        return NULL;

    t_ast_node *left = parse_command(tokens);
    if (!left)
        return NULL;

    if (*tokens)
    {
        t_token *current = (t_token *)(*tokens)->content;
        if (current && current->type == TOKEN_PIPE)
        {
            *tokens = (*tokens)->next;  // Move past the pipe
            t_ast_node *right = parse_command(tokens);
            if (right)
            {
                t_ast_node *pipe_node = create_ast_node(NODE_PIPE, NULL);
                pipe_node->left = left;
                pipe_node->right = right;
                return pipe_node;
            }
            else
            {
                free_ast(left);  // Free the left node if parsing the right side fails
                return NULL;
            }
        }
    }
    return left;
}

void print_ast(t_ast_node *node, int depth)
{
    if (!node)
        return;

    // Print indentation based on depth
    for (int i = 0; i < depth; i++)
        printf("  ");

    // Print the node type (as a number) and data
    printf("Type: %d", node->type);
    if (node->data)
        printf(", Data: %s", node->data);
    printf("\n");

    // Recursively print left and right children
    print_ast(node->left, depth + 1);
    print_ast(node->right, depth + 1);
}

// Wrapper function to print the AST tree
void print_ast_tree(t_ast_node *root)
{
    if (!root)
    {
        printf("AST is empty.\n");
        return;
    }
    printf("Abstract Syntax Tree (AST):\n");
    print_ast(root, 0);
}
