#include "minishell.h"

t_list *handle_invalid_redirection(t_list **redir)
{
	ft_lstclear(redir, free); // Clear the redirection list
	return NULL;
}
t_ast_node *create_pipe_node(t_list *tokens, t_ast_node *left)
{
	t_ast_node *pipe_node;

	tokens = tokens->next; // Move to the next token after the pipe
	pipe_node = create_node(NODE_PIPE);
	if (!pipe_node)
	{
		free_ast(left);
		return NULL;
	}

	pipe_node->left = left;
	pipe_node->right = parse_pipe(tokens);

	if (!pipe_node->right)
	{
		free_ast(pipe_node->left);
		free(pipe_node);
		return NULL;
	}

	return pipe_node;
}

t_list *get_redir(t_list **tokens)
{
	t_list *redir;
	t_list *tmp_list;
	t_token *current_token;

	redir = NULL;
	tmp_list = *tokens;

	while (tmp_list != NULL && is_redirection_token(((t_token *)tmp_list->content)->type) &&
		   ((t_token *)tmp_list->content)->type != TOKEN_PIPE)
	{
		current_token = (t_token *)tmp_list->content;
		tmp_list = tmp_list->next; // Advance to the next token (file name)

		if (validate_redirection(tmp_list))
			tmp_list = process_redirection(&redir, current_token, tmp_list);
		else
			return handle_invalid_redirection(&redir);
	}

	*tokens = tmp_list; // Update the tokens pointer to the new position
	return redir;
}
char *get_argument(t_list **tmp_list, char *tmp, t_list *tokens)
{
    char *arg;

    if (token_content(*tmp_list)->space == 0 && (*tmp_list)->next != NULL && *tmp_list != tokens)
        arg = join_tokens(tmp_list, ft_strdup(token_content(*tmp_list)->token));
    else if (ft_strcmp(token_content(*tmp_list)->token, tmp) == 0 && (*tmp_list)->next != NULL)
        arg = join_tokens(tmp_list, ft_strdup(token_content(*tmp_list)->token));
    else
        arg = ft_strdup(token_content(*tmp_list)->token);

    return arg;
}

int allocate_cmd_args(char ***cmd_arg, int arg_count, char **tmp)
{
    *cmd_arg = malloc((arg_count + 2) * sizeof(char *));
    if (!(*cmd_arg))
    {
        free(*tmp);
        return 0;
    }
    return 1;
}
