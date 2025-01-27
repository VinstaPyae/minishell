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
        	printf("PIPE : %s\n", ((t_token *)(*tokens)->content)->token);
		*tokens = (*tokens)->next;
		p_node = create_node(NODE_PIPE);
		if (!p_node)
			return (free_ast(left), NULL);
		p_node->left = left;
		p_node->right = parse_pipe(tokens);
		if (!p_node->right)
			return (free_ast(p_node), NULL);
	}
	else
	{
		// printf("Return ONLY CMD\n");
		return (left);
	}
	return (p_node);
}

t_ast_node	*parse_cmd(t_list **tokens)
{
	t_ast_node	*cmd_node;

	cmd_node = create_node(NODE_COMMAND);
	if (!cmd_node)
		return (NULL);
	while (((*tokens) != NULL && ((t_token *)(*tokens)->content)->type != TOKEN_PIPE))
	{
		if ((*tokens) != NULL && is_word_token(((t_token *)(*tokens)->content)->type))
		{
			if (!cmd_node->cmd)
			{
				cmd_node->cmd = ft_strdup(((t_token *)(*tokens)->content)->token);
				if (!cmd_node->cmd)
					return (free(cmd_node->cmd), NULL);
				*tokens = (*tokens)->next;
				//printf("Lee cmd: %s\n", cmd_node->cmd);
			}
			else
			{
				cmd_node->cmd_arg = get_cmd_args(tokens);
				//printf("cmd: %s\n", cmd_node->cmd_arg[0]);
				if (!cmd_node->cmd_arg)
							return (free_ast(cmd_node), NULL);
			}
		}
		if ((*tokens) != NULL && is_redirection_token(((t_token *)(*tokens)->content)->type))
		{
			cmd_node->redir = get_redir(tokens);
			if (!cmd_node->redir)
				return (free_ast(cmd_node), NULL);
			//print_redir(cmd_node->redir);
		}
	}
	return (cmd_node);
}

/* char	*get_cmd(t_list **tokens)
{
	char	*c_cmd;
	char	*cmd;
	int		i;
	int		j;

	if (!(*tokens))
		return (NULL);
	i = 0;
	j = 0;
	c_cmd = (((t_token *)(*tokens)->content)->token);
	if (!c_cmd)
		return (NULL);
	while (c_cmd[i] && !isspace(c_cmd[i]))
		i++;
	cmd = malloc((i + 1) * sizeof(char));
	if (!cmd)
		return (NULL);
	while (j < i)
	{
		cmd[j] = c_cmd[j];
		j++;
	}
	cmd[i] = '\0';
	*tokens = (*tokens)->next;
	return (cmd);
} */

char	**get_cmd_args(t_list **tokens)
{
	t_list	*tmp_list;
	char	**cmd_arg;
	//char	**arg;
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
		/* arg[i] = ft_strdup(token_content(tmp_list)->token);
		if (!cmd_arg[i])
		{
			// Free previously allocated strings if allocation fails
			while (i > 0)
				free(cmd_arg[--i]);
			free(cmd_arg);
			return NULL;
		} */
		//printf("lee space: %d\n", token_content(tmp_list)->space);
		if (token_content(tmp_list)->space > 0)
		{
			cmd_arg[i] = ft_strjoin(token_content(tmp_list)->token, " ");
		}
		else
		{
			cmd_arg[i] = ft_strdup(token_content(tmp_list)->token);
		}
		i++;
		tmp_list = tmp_list->next;
		//printf("cmd: %s\n", cmd_arg[i]);
	}
	cmd_arg[i] = NULL;
	(*tokens) = tmp_list;
	return (cmd_arg);
}

t_ast_node *create_node(t_node_type type)
{
	t_ast_node *node = malloc(sizeof(t_ast_node));
	if (!node) {
		perror("malloc"); // Print error if malloc fails
		exit(EXIT_FAILURE); // Exit the program
	}
	node->type = type;
	node->cmd = NULL;
	node->cmd_arg = NULL;      // Arguments list is initially NULL
	node->redir = NULL;
	node->left = NULL;      // Left child is NULL
	node->right = NULL;     // Right child is NULL
	return node; // Return the newly created node
}

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