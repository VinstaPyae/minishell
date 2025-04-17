#include "minishell.h"

static t_ast_node *create_pipe_node(t_list *tokens, t_ast_node *left)
{
    tokens = tokens->next; // Move to the next token after the pipe
    t_ast_node *pipe_node = create_node(NODE_PIPE);
    if (!pipe_node)
        return (free_ast(left), NULL);

    pipe_node->left = left;
    pipe_node->right = parse_pipe(tokens);

    if (!pipe_node->right)
        return (free_ast(pipe_node->left), free(pipe_node), NULL);

    return pipe_node;
}

static int is_pipe_token(t_list *tokens)
{
    return (tokens != NULL && token_content(tokens)->type == TOKEN_PIPE);
}

t_ast_node *parse_pipe(t_list *tokens)
{
    if (!tokens)
        return NULL;

    t_ast_node *left = parse_cmd(&tokens);
    if (!left)
        return NULL;

    if (is_pipe_token(tokens))
        return create_pipe_node(tokens, left);

    return left; // Return the command node if no pipe is found
}

static int process_redirections(t_ast_node *cmd_node, t_list **tokens)
{
    cmd_node->redir = get_redir(tokens);
    if (!cmd_node->redir)
        return 0; // Return failure if memory allocation fails
    return 1; // Return success
}

static int process_command_arguments(t_ast_node *cmd_node, t_list **tokens)
{
    char **tmp_arg;
    char *trimmed;

    /* get the argv array */
    cmd_node->cmd_arg = get_cmd_args(tokens);
    if (!cmd_node->cmd_arg)
        return 0;  // allocation failure

    tmp_arg = cmd_node->cmd_arg;
    if (ft_strchr(tmp_arg[0], ' ') != NULL)
    {
        /* first produce the trimmed copy */
        trimmed = ft_strtrim(tmp_arg[0], " ");
        if (!trimmed)
            return 0;  // allocation failure

        /* then free the old untrimmed string and replace it */
        free(tmp_arg[0]);
        tmp_arg[0] = trimmed;
    }

    /* do NOT free_arg(tmp_arg) here — that belongs in your cleanup path,
    once you’re completely done with cmd_node->cmd_arg. */

    return 1;  // success

}

static t_ast_node *create_command_node(void)
{
    t_ast_node *cmd_node = create_node(NODE_COMMAND);
    if (!cmd_node)
        return NULL;
    return cmd_node;
}

t_ast_node *parse_cmd(t_list **tokens)
{
    t_ast_node *cmd_node = create_command_node();
    if (!cmd_node)
        return NULL;

    while ((*tokens) != NULL && !is_pipe_token((*tokens))) {
        if (is_word_token(token_content(*tokens)->type)) {
            if (!process_command_arguments(cmd_node, tokens))
                return free_ast(cmd_node), NULL;
        } else if (is_redirection_token(token_content(*tokens)->type)) {
            if (!process_redirections(cmd_node, tokens))
                return free_ast(cmd_node), NULL;
        }
    }
    return cmd_node;
}

static int count_arguments(t_list *tokens)
{
    int count = 1; // Start at 1 for the first argument
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
    char *tmp2;

    while ((*tmp_list)->next != NULL && token_content(*tmp_list)->space == 0 &&
           is_word_token(token_content((*tmp_list)->next)->type))
    {
        tmp = ft_strjoin(arg, token_content((*tmp_list)->next)->token);
        if (!tmp)
            return (free(arg), NULL);
        if (token_content((*tmp_list)->next)->space > 0 && (*tmp_list)->next->next != NULL &&
            is_word_token(token_content((*tmp_list)->next->next)->type))
        {
            tmp2 = ft_strjoin(tmp, " ");
            if (!tmp2)
                return (free(tmp), free(arg), NULL);
            free(tmp);
            tmp = tmp2;
        }
        free(arg);
        arg = tmp;
        *tmp_list = (*tmp_list)->next;
    }
    return arg;
}

static char *get_argument(t_list **tmp_list)
{
    char *arg;

    if (token_content(*tmp_list)->space > 0 && (*tmp_list)->next != NULL &&
        is_word_token(token_content((*tmp_list)->next)->type))
        arg = ft_strjoin(token_content(*tmp_list)->token, " ");
    else
        arg = ft_strdup(token_content(*tmp_list)->token);
    if (!arg)
        return NULL;
    arg = join_tokens(tmp_list, arg);
    return arg;
}

char **get_cmd_args(t_list **tokens)
{
    t_list *tmp_list;
    char **cmd_arg;
    char *arg;
    int i;

    if (!tokens || !(*tokens))
        return (NULL);
    int arg_count = count_arguments(*tokens);
    cmd_arg = malloc((arg_count + 1) * sizeof(char *));
    if (!cmd_arg)
        return (NULL);
    i = 0;
    tmp_list = *tokens;
    while (tmp_list != NULL && is_word_token(token_content(tmp_list)->type) &&
           token_content(tmp_list)->type != TOKEN_PIPE)
    {
        arg = get_argument(&tmp_list);
        if (!arg)
            return (free_arg(cmd_arg), NULL);
        cmd_arg[i++] = arg;
        tmp_list = tmp_list->next;
    }
    cmd_arg[i] = NULL;
    *tokens = tmp_list;
    return cmd_arg;
}

t_ast_node *create_node(t_node_type type)
{
	t_ast_node *node = malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = type;
	// node->cmd = NULL;
	node->cmd_arg = NULL; // Arguments list is initially NULL
	node->redir = NULL;
	node->left = NULL;	// Left child is NULL
	node->right = NULL; // Right child is NULL
	return node;		// Return the newly created node
}

static int validate_redirection(t_list *tmp_list)
{
    return (tmp_list != NULL && is_word_token(((t_token *)tmp_list->content)->type) &&
            ((t_token *)tmp_list->content)->type != TOKEN_PIPE);
}

static t_list *process_redirection(t_list **redir, t_token *current_token, t_list *tmp_list)
{
    t_list *new_redir = create_redir(((t_token *)tmp_list->content)->token, current_token->type);
    if (!new_redir)
    {
        ft_lstclear(redir, free); // Clear the redirection list on failure
        return NULL;
    }
    ft_lstadd_back(redir, new_redir); // Add the new redirection to the list
    return tmp_list->next; // Advance to the next token
}

static t_list *handle_invalid_redirection(t_list **redir)
{
    ft_lstclear(redir, free); // Clear the redirection list
    return NULL;
}

t_list *get_redir(t_list **tokens)
{
    t_list *redir = NULL;
    t_list *tmp_list = *tokens;

    while (tmp_list != NULL && is_redirection_token(((t_token *)tmp_list->content)->type) &&
           ((t_token *)tmp_list->content)->type != TOKEN_PIPE)
    {
        t_token *current_token = (t_token *)tmp_list->content;
        tmp_list = tmp_list->next; // Advance to the next token (file name)

        if (validate_redirection(tmp_list))
            tmp_list = process_redirection(&redir, current_token, tmp_list);
        else
            return handle_invalid_redirection(&redir);
    }

    *tokens = tmp_list; // Update the tokens pointer to the new position
    return redir;
}
