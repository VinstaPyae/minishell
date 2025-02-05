#include "minishell.h"

t_ast_node	*parse_pipe(t_list *tokens)
{
	t_ast_node	*left;
	t_ast_node	*p_node;

	if (!(tokens))
		return (NULL);
	left = parse_cmd(&tokens);
	if (!left)
		return (NULL);
	p_node = NULL;
	if ((tokens) != NULL && (token_content(tokens)->type == TOKEN_PIPE))
	{
        	printf("PIPE : %s\n", (token_content(tokens)->token));
		tokens = tokens->next;
		p_node = create_node(NODE_PIPE);
		if (!p_node)
			return (free_ast(left), NULL);
		p_node->left = left;
		p_node->right = parse_pipe(tokens);
		if (!p_node->right)
			return (free_ast(p_node->left), free(p_node), NULL);
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
					return (free(cmd_node), NULL);
				*tokens = (*tokens)->next;
				//printf("Lee cmd: %s\n", cmd_node->cmd);
			}
			else
			{
				cmd_node->cmd_arg = get_cmd_args(tokens);
				// printf("cmd: %s\n", cmd_node->cmd_arg[0]);
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
	int		i;

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

/*char	**get_cmd_args(t_list **tokens)
{
	t_list	*tmp_list;
	char	**cmd_arg;
	int		i;

	i = 0;
	tmp_list = (*tokens);
	while (*tokens != NULL && is_word_token(((t_token *)(*tokens)->content)->type) &&
	((t_token *)(*tokens)->content)->type != TOKEN_PIPE)
	{
		if (((t_token *)(*tokens)->content)->space > 0 && (*tokens)->next != NULL)
			i++;
		*tokens = (*tokens)->next;
	}
	//printf("Arg: %d\n", i);
	cmd_arg = malloc((i + 1) * sizeof(char *));
	if (!cmd_arg)
		return (NULL);
	i = 0;
	while ((tmp_list) != NULL && is_word_token(token_content(tmp_list)->type) &&
	token_content(tmp_list)->type != TOKEN_PIPE)
	{
		// printf("token type: %d\n", token_content(tmp_list)->type);
		// printf("token vale: %s\n", token_content(tmp_list)->token);
		// printf("token space: %d\n", token_content(tmp_list)->space);
		if (token_content(tmp_list)->space > 0 && tmp_list->next != NULL)
		{
			cmd_arg[i] = ft_strjoin(token_content(tmp_list)->token, " ");
			// printf("arg%d: %s\n", i, cmd_arg[i]);
		}
		else
		{
			if (tmp_list->next == NULL)
			{
				cmd_arg[i] = ft_strdup(token_content(tmp_list)->token);
				// printf("arg%d: %s\n", i, cmd_arg[i]);
			}
			else
			{
				cmd_arg[i] = ft_strjoin(token_content(tmp_list)->token, token_content(tmp_list->next)->token);
				tmp_list = tmp_list->next;
				while (token_content(tmp_list)->space == 0 && tmp_list->next != NULL)
				{
					cmd_arg[i] = ft_strjoin(cmd_arg[i], token_content(tmp_list->next)->token);
					tmp_list = tmp_list->next;
				}
			}
		}
		i++;
		tmp_list = tmp_list->next;
		//printf("cmd: %s\n", cmd_arg[i]);
	}
	cmd_arg[i] = NULL;
	(*tokens) = tmp_list;
	return (cmd_arg);
}*/

char	**get_cmd_args(t_list **tokens)
{
	t_list	*tmp_list;
	char	**cmd_arg;
	char	*arg;
	char	*tmp;
	int		i;

	if (!tokens || !(*tokens))
		return (NULL);

	i = 1; // Start at 1 for the first argument
	tmp_list = (*tokens);

	// First, count how many arguments we need
	while (tmp_list != NULL && is_word_token(token_content(tmp_list)->type) &&
		token_content(tmp_list)->type != TOKEN_PIPE)
	{
		if (token_content(tmp_list)->space > 0 && tmp_list->next != NULL)
			i++; // Count separate arguments
		tmp_list = tmp_list->next;
	}

	cmd_arg = malloc((i + 1) * sizeof(char *));
	if (!cmd_arg)
		return (NULL);

	i = 0;
	tmp_list = (*tokens);

	// Now, populate the cmd_arg array
	while (tmp_list != NULL && is_word_token(token_content(tmp_list)->type) &&
		token_content(tmp_list)->type != TOKEN_PIPE)
	{
		if (token_content(tmp_list)->space > 0 && tmp_list->next != NULL)
			arg = ft_strjoin(token_content(tmp_list)->token, " ");
		else
			arg = ft_strdup(token_content(tmp_list)->token);
		if (!arg)
			return (free_arg(cmd_arg),NULL); // Handle allocation failure properly

		while (tmp_list->next != NULL && token_content(tmp_list->next)->space == 0 &&
			is_word_token(token_content(tmp_list->next)->type) && token_content(tmp_list)->type != TOKEN_PIPE)
		{
			tmp = ft_strjoin(arg, token_content(tmp_list->next)->token);
			if (!tmp)
				return (free(arg), free_arg(cmd_arg), NULL);
			free(arg);
			arg = tmp;
			tmp_list = tmp_list->next;
		}
		cmd_arg[i++] = arg;
		tmp_list = tmp_list->next;
	}

	cmd_arg[i] = NULL;
	(*tokens) = tmp_list;
	return (cmd_arg);
}


t_ast_node *create_node(t_node_type type)
{
	t_ast_node *node = malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
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
