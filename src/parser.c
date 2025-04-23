#include "minishell.h"

static int is_pipe_token(t_list *tokens)
{
	int result;

	result = (tokens != NULL && token_content(tokens)->type == TOKEN_PIPE);
	return result;
}

static int process_redirections(t_ast_node *cmd_node, t_list **tokens)
{
	t_list *redir;
	t_list *temp;

	redir = get_redir(tokens);
	if (!redir)
		return 0; // Return failure if memory allocation fails

	if (cmd_node->redir)
	{
		temp = cmd_node->redir;
		while (temp->next)
			temp = temp->next;
		temp->next = redir;
	}
	else
	{
		cmd_node->redir = redir;
	}
	return 1; // Return success
}


// Helper to count elements in a NULL-terminated char**
int count_strs(char **arr)
{
    int i = 0;
    while (arr && arr[i])
        i++;
    return i;
}

// Function to join two char** arrays
char **join_args(char **arg, char **new)
{
    int arg_len = count_strs(arg);
    int new_len = count_strs(new);
    int total_len = arg_len + new_len;
    char **joined = malloc(sizeof(char *) * (total_len + 1));
    int i = 0;

    if (!joined)
        return NULL;

    // Copy arg strings
    while (i < arg_len)
    {
        joined[i] = ft_strdup(arg[i]);
        if (!joined[i]) // handle strdup failure
        {
            free_arg(joined);
            return NULL;
        }
        i++;
    }

    // Copy new strings
    int j = 0;
    while (j < new_len)
    {
        joined[i] = ft_strdup(new[j]);
        if (!joined[i])
        {
            free_arg(joined);
            return NULL;
        }
        i++;
        j++;
    }

    // Null-terminate
    joined[i] = NULL;

    // Optionally free old arrays if not needed
    free_arg(arg);
    free_arg(new);

    return joined;
}

static int process_command_arguments(t_ast_node *cmd_node, t_list **tokens)
{
	char **tmp_arg;
	char *trimmed;
	char **new_arg;
	char **arg;
	t_list *tmp_list;

	tmp_list = *tokens;
	arg = cmd_node->cmd_arg;
	if (arg)
	{
		new_arg = get_cmd_args(tokens);
		if (!new_arg)
			return 0;
		cmd_node->cmd_arg = join_args(arg, new_arg);
	}
	else
		cmd_node->cmd_arg = get_cmd_args(tokens);
	if (!cmd_node->cmd_arg)
		return 0; // Allocation failure

	tmp_arg = cmd_node->cmd_arg;
	if (ft_strchr(tmp_arg[0], ' ') != NULL && token_content(tmp_list)->type != TOKEN_DQUOTE && token_content(tmp_list)->type != TOKEN_SQUOTE)
	{
		trimmed = ft_strtrim(tmp_arg[0], " ");
		if (!trimmed)
			return 0; // Allocation failure
		free(tmp_arg[0]);
		tmp_arg[0] = trimmed;
	}
	return 1; // Success
}

static t_ast_node *create_command_node(void)
{
	t_ast_node *cmd_node;

	cmd_node = create_node(NODE_COMMAND);
	if (!cmd_node)
		return NULL;
	return cmd_node;
}

static int count_arguments(t_list *tokens)
{
	int count;

	count = 1; // Start at 1 for the first argument
	while (tokens != NULL && is_word_token(token_content(tokens)->type) &&
			token_content(tokens)->type != TOKEN_PIPE)
	{
		if (token_content(tokens)->space > 0 && tokens->next != NULL)
			count++; // Count separate arguments
		tokens = tokens->next;
	}
	return count;
}

static char *join_tokens(t_list **tmp_list, char *arg)
{
	char *tmp;

	while ((*tmp_list)->next != NULL && token_content(*tmp_list)->space == 0 &&
			is_word_token(token_content((*tmp_list)->next)->type))
	{
		tmp = ft_strjoin(arg, token_content((*tmp_list)->next)->token);
		if (!tmp)
		{
			free(arg);
			return NULL;
		}
		free(arg);
		arg = tmp;
		*tmp_list = (*tmp_list)->next;
	}
	return arg;
}

// static char *get_argument(t_list **tmp_list)
// {
// 	char *arg;

// 	if (token_content(*tmp_list)->space > 0 && (*tmp_list)->next != NULL &&
// 		is_word_token(token_content((*tmp_list)->next)->type))
// 		arg = ft_strjoin(token_content(*tmp_list)->token, " ");
// 	else
// 		arg = ft_strdup(token_content(*tmp_list)->token);
// 	if (!arg)
// 		return NULL;
// 	arg = join_tokens(tmp_list, arg);
// 	return arg;
// }

static int validate_redirection(t_list *tmp_list)
{
	int result;

	result = (tmp_list != NULL && is_word_token(((t_token *)tmp_list->content)->type) &&
			  ((t_token *)tmp_list->content)->type != TOKEN_PIPE);
	return result;
}

static t_list *process_redirection(t_list **redir, t_token *current_token, t_list *tmp_list)
{
	t_list *new_redir;

	new_redir = create_redir(((t_token *)tmp_list->content)->token, current_token->type);
	if (!new_redir)
	{
		ft_lstclear(redir, free); // Clear the redirection list on failure
		return NULL;
	}
	ft_lstadd_back(redir, new_redir); // Add the new redirection to the list
	return tmp_list->next;           // Advance to the next token
}

static t_list *handle_invalid_redirection(t_list **redir)
{
	ft_lstclear(redir, free); // Clear the redirection list
	return NULL;
}
static t_ast_node *create_pipe_node(t_list *tokens, t_ast_node *left)
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

char **get_cmd_args(t_list **tokens)
{
	t_list *tmp_list;
	char **cmd_arg;
	char *arg;
	char *tmp;
	int i;
	int arg_count;

	tmp = ft_strdup("");
	if (!tmp)
		return NULL;

	if (!tokens || !(*tokens))
	{
		free(tmp);
		return NULL;
	}

	arg_count = count_arguments(*tokens);
	cmd_arg = malloc((arg_count + 2) * sizeof(char *));
	if (!cmd_arg)
	{
		free(tmp);
		return NULL;
	}

	i = 0;
	tmp_list = *tokens;
	while (tmp_list != NULL && is_word_token(token_content(tmp_list)->type) &&
		   token_content(tmp_list)->type != TOKEN_PIPE)
	{
		if (token_content(tmp_list)->space == 0 && tmp_list->next != NULL && tmp_list != *tokens)
			arg = join_tokens(&tmp_list, ft_strdup(token_content(tmp_list)->token));
		else if (ft_strcmp(token_content(tmp_list)->token, tmp) == 0 && tmp_list->next != NULL)
			arg = join_tokens(&tmp_list, ft_strdup(token_content(tmp_list)->token));
		else
			arg = ft_strdup(token_content(tmp_list)->token);

		if (!arg)
		{
			free_arg(cmd_arg);
			free(tmp);
			return NULL;
		}

		cmd_arg[i++] = arg;
		tmp_list = tmp_list->next;
	}

	free(tmp); // Free the temporary string
	cmd_arg[i] = NULL;
	*tokens = tmp_list;
	return cmd_arg;
}

t_ast_node *create_node(t_node_type type)
{
	t_ast_node *node;

	node = malloc(sizeof(t_ast_node));
	if (!node)
		return NULL;

	node->type = type;
	node->cmd_arg = NULL; // Arguments list is initially NULL
	node->redir = NULL;
	node->left = NULL;    // Left child is NULL
	node->right = NULL;   // Right child is NULL
	return node;          // Return the newly created node
}

t_ast_node *parse_pipe(t_list *tokens)
{
	t_ast_node *left;

	if (!tokens)
		return NULL;

	left = parse_cmd(&tokens);
	if (!left)
		return NULL;

	if (is_pipe_token(tokens))
		return create_pipe_node(tokens, left);

	return left; // Return the command node if no pipe is found
}

t_ast_node *parse_cmd(t_list **tokens)
{
	t_ast_node *cmd_node;

	cmd_node = create_command_node();
	if (!cmd_node)
		return NULL;

	while ((*tokens) != NULL && !is_pipe_token((*tokens)))
	{
		if (is_word_token(token_content(*tokens)->type))
		{
			if (!process_command_arguments(cmd_node, tokens))
			{
				free_ast(cmd_node);
				return NULL;
			}
		}
		else if (is_redirection_token(token_content(*tokens)->type))
		{
			if (!process_redirections(cmd_node, tokens))
			{
				free_ast(cmd_node);
				return NULL;
			}
		}
	}
	return cmd_node;
}