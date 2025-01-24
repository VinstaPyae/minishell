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
	while (((*tokens) != NULL && ((t_token *)(*tokens)->content)->type != TOKEN_PIPE))
	{
		if ((*tokens) != NULL && is_word_token(((t_token *)(*tokens)->content)->type))
			cmd_node->cmd_arg = get_cmd(tokens);
        if ((*tokens) != NULL)
        printf("token after cmd : %s\n", ((t_token *)(*tokens)->content)->token);
		if ((*tokens) != NULL && is_redirection_token(((t_token *)(*tokens)->content)->type))
        {
            cmd_node->redir = get_redir(tokens);
            t_list *redir_list = cmd_node->redir;
            if (redir_list != NULL)
            {
                print_redir(redir_list);
                // Free the redir list after use
                ft_lstclear(&redir_list, free);
            }
        }
	}
	return (cmd_node);
}

char	**get_cmd(t_list **tokens)
{
	t_list	*tmp_list;
	char	**cmd_arg;
	int		i;

	i = 0;
	tmp_list = (*tokens);
	while (*tokens != NULL && is_word_token(((t_token *)(*tokens)->content)->type) &&
	((t_token *)(*tokens)->content)->type != TOKEN_PIPE)
	{
		*tokens = (*tokens)->next;
		i++;
	}
	cmd_arg = malloc((i + 1) * sizeof(char *));
	if (!cmd_arg)
		return (NULL);
	i = 0;
	while ((tmp_list) != NULL && is_word_token(token_content(tmp_list)->type) &&
	token_content(tmp_list)->type != TOKEN_PIPE)
	{
		cmd_arg[i] = ft_strdup(token_content(tmp_list)->token);
		tmp_list = tmp_list->next;
		printf("cmd: %s\n", cmd_arg[i]);
	}
	cmd_arg[i] = NULL;
    (*tokens) = tmp_list;
	return (cmd_arg);
}

// t_list	**get_redir(t_list **tokens)
// {
// 	t_list	**redir;
// 	t_list	*new_redir;
// 	t_list	*tmp_list;

// 	redir = NULL;
// 	tmp_list = (*tokens);
// 	while (*tokens != NULL && is_redirection_token(((t_token *)(*tokens)->content)->type) &&
// 	((t_token *)(*tokens)->content)->type != TOKEN_PIPE)
// 	{
// 		*tokens = (*tokens)->next;
// 		if (*tokens != NULL && is_word_token(((t_token *)(*tokens)->content)->type) &&
// 		((t_token *)(*tokens)->content)->type != TOKEN_PIPE)(*tokens) != NULL
// 			new_redir = create_redir(((t_token *)(*tokens)->content)->token,token_content(tmp_list)->type );
// 		if (!new_redir)printf("token after cmd: %s\n", ((t_token *)(*tokens)->content)->token);
// 			return (ft_lstclear(&new_redir, free), NULL);
// 		ft_lstadd_back(redir, new_redir);
// 		tmp_list = tmp_list->next;
// 		printf("Redir Type: %d, File: %s\n", ((t_redir *)(*redir)->content)->type, ((t_redir *)(*redir)->content)->file);
// 	}
// 	return (redir);
// }

/* t_list **get_redir(t_list **tokens)
{
    t_list *tmp_list = *tokens;
    t_list **redir = NULL; // Initialize the list of redirections
    t_list *new_redir;

    while (tmp_list != NULL && is_redirection_token(((t_token *)(tmp_list->content))->type))
    {void print_redir(t_list *redir)if ((*tokens) != NULL && is_redirection_token(((t_token *)(*tokens)->content)->type))
			cmd_node->redir = get_redir(tokens);
    {
        t_redir *r = (t_redir *)current->content;
        printf("Redir Type: %d, File: %s\n", r->type, r->file);
        current = current->next;
    }
}
        t_token *redir_token = (t_token *)(tmp_list->content);(*tokens) != NULL
        tmp_list = tmp_list->next; // Move to the next token (file name or invalid token)

        if (tmp_list != NULL && is_word_token(((t_token *)(tmp_list->content))->type))
        {
            t_token *file_token = (t_token *)(tmp_list->content);
            new_redir = create_redir(file_token->token, redir_token->type);
            if (!new_redir)
            {
                // Handle memory allocation failure
                ft_lstclear(&new_redir, free);
                return NULL;
            }
            ft_lstadd_back(redir, new_redir); // Add the new redirection to the list
            tmp_list = tmp_list->next; // Move past the file token
        }
		printf("Redir Type: %d, File: %s\n", ((t_redir *)(*redir)->(*tokens) != NULLcontent)->type, ((t_redir *)(*redir)->content)->file);
    }
    *tokens = tmp_list; // Update the tokens list pointer
    return redir;
} */


t_ast_node *create_node(t_node_type type)
{
    t_ast_node *node = malloc(sizeof(t_ast_node));
    if (!node) {
        perror("malloc"); // Print error if malloc fails
        exit(EXIT_FAILURE); // Exit the program
    }
    node->type = type;
    node->cmd_arg = NULL;      // Arguments list is initially NULL
    node->redir = NULL;
    node->left = NULL;      // Left child is NULL
    node->right = NULL;     // Right child is NULL
    return node; // Return the newly created node
}

/* t_list **get_redir(t_list **tokens)
{
    t_list **redir;
    t_list *new_redir;
    t_list *tmp_list;

    // Allocate memory for the redir list
    redir = malloc(sizeof(t_list *));
    if (redir == NULL)
        return NULL;
    *redir = NULL;

    tmp_list = *tokens;
    while (tmp_list != NULL && is_redirection_token(((t_token *)tmp_list->content)->type) &&
           ((t_token *)tmp_list->content)->type != TOKEN_PIPE)
    {
        t_token *current_token = (t_token *)tmp_list->content;
        tmp_list = tmp_list->next;

        if (tmp_list != NULL && is_word_token(((t_token *)tmp_list->content)->type) &&
            ((t_token *)tmp_list->content)->type != TOKEN_PIPE)
        {
            new_redir = create_redir(((t_token *)tmp_list->content)->token, current_token->type);
            if (!new_redir)
            {
                ft_lstclear(redir, free);
                free(redir);
                return NULL;
            }
            ft_lstadd_back(redir, new_redir);
        }
        tmp_list = tmp_list->next;
    }
    (*tokens) = tmp_list;
    return redir;
}
 */

t_list *get_redir(t_list **tokens)
{
    t_list *redir = NULL; // Use a single pointer for the redirection list
    t_list *new_redir;
    t_list *tmp_list;

    tmp_list = *tokens;
    while (tmp_list != NULL && is_redirection_token(((t_token *)tmp_list->content)->type) &&
           ((t_token *)tmp_list->content)->type != TOKEN_PIPE)
    {
        t_token *current_token = (t_token *)tmp_list->content;
        tmp_list = tmp_list->next; // Advance to the next token (file name)

        // Ensure the next token is a word token (file name)
        if (tmp_list != NULL && is_word_token(((t_token *)tmp_list->content)->type) &&
            ((t_token *)tmp_list->content)->type != TOKEN_PIPE)
        {
            new_redir = create_redir(((t_token *)tmp_list->content)->token, current_token->type);
            if (!new_redir)
            {
                ft_lstclear(&redir, free); // Clear the redirection list on failure
                return NULL;
            }
            ft_lstadd_back(&redir, new_redir); // Add the new redirection to the list
        }
        else
        {
            // Invalid redirection (missing file name), handle error
            ft_lstclear(&redir, free);
            return NULL;
        }

        tmp_list = tmp_list->next; // Advance to the next token after processing the redirection
    }

    // Update the tokens pointer to the new position
    *tokens = tmp_list;
    return redir;
}