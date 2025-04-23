#include "minishell.h"

// Main function to join two char** arrays
char **join_args(char **arg, char **new)
{
    int total_len;
    char **joined;
    int index;

    total_len = calculate_total_length(arg, new);
    joined = malloc(sizeof(char *) * (total_len + 1));
    if (!joined)
        return NULL;

    index = 0;
    if (!duplicate_strings(joined, arg, &index))
        return NULL;

    if (!duplicate_strings(joined, new, &index))
        return NULL;

    joined[index] = NULL;

    free_arg(arg);
    free_arg(new);

    return joined;
}

int handle_existing_arguments(t_ast_node *cmd_node, t_list **tokens)
{
    char **new_arg;

    new_arg = get_cmd_args(tokens);
    if (!new_arg)
        return 0;
    cmd_node->cmd_arg = join_args(cmd_node->cmd_arg, new_arg);
    if (!cmd_node->cmd_arg)
        return 0; // Allocation failure
    return 1;
}

int handle_new_arguments(t_ast_node *cmd_node, t_list **tokens)
{
    cmd_node->cmd_arg = get_cmd_args(tokens);
    if (!cmd_node->cmd_arg)
        return 0; // Allocation failure
    return 1;
}

int trim_first_argument(char **tmp_arg, t_list *tmp_list)
{
    char *trimmed;

    if (ft_strchr(tmp_arg[0], ' ') != NULL &&
        token_content(tmp_list)->type != TOKEN_DQUOTE &&
        token_content(tmp_list)->type != TOKEN_SQUOTE)
    {
        trimmed = ft_strtrim(tmp_arg[0], " ");
        if (!trimmed)
            return 0; // Allocation failure
        free(tmp_arg[0]);
        tmp_arg[0] = trimmed;
    }
    return 1;
}

int process_command_arguments(t_ast_node *cmd_node, t_list **tokens)
{
    char **tmp_arg;
    t_list *tmp_list;
    int result;

    tmp_list = *tokens;
    if (cmd_node->cmd_arg)
        result = handle_existing_arguments(cmd_node, tokens);
    else
        result = handle_new_arguments(cmd_node, tokens);

    if (!result)
        return 0;

    tmp_arg = cmd_node->cmd_arg;
    if (!trim_first_argument(tmp_arg, tmp_list))
        return 0;

    return 1; // Success
}
