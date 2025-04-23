#include "minishell.h"

static void process_split_values(t_token *token, t_list **current, char **split_value, int space1)
{
    t_list *insert_pos;
    int i;

    i = 0;
    insert_pos = *current;
    while (split_value && split_value[i])
    {
        if (i == 0)
        {
            handle_first_split_value(token, split_value, i, space1);
        }
        else
        {
            insert_pos = handle_remaining_split_values(insert_pos, split_value, i, space1);
            if (!insert_pos)
                break;
        }
        i++;
    }
    *current = insert_pos;
}

static void insert_split_values(t_token *token, t_list **current, char **split_value, int space1)
{
    process_split_values(token, current, split_value, space1);
}

static void process_expanded_values(t_token *token, t_list **current, char **expanded_value, int space1)
{
    char *result;
    char **split_value;

    result = join_expanded_values(expanded_value);
    free_arg(expanded_value);

    split_value = split_expanded_value(result);
    split_value = handle_split_value(result, split_value);
    free(result);

    insert_split_values(token, current, split_value, space1);
    free_arg(split_value);
}

static void process_variable_token(t_token *token, t_list **current, t_minishell *shell)
{
    char **expanded_value;
    int space1;

    expanded_value = expand_variable(token->token, shell);
    if (!expanded_value || !expanded_value[0])
    {
        handle_empty_expansion(token);
        free_arg(expanded_value);
        return;
    }

    space1 = token->space;
    process_expanded_values(token, current, expanded_value, space1);
}

void expand_tokens(t_minishell *shell)
{
    t_list *current;

    current = shell->l_token;
    while (current)
    {
        t_token *token;

        token = (t_token *)current->content;
        if (token->type == TOKEN_VARIABLE)
        {
            process_variable_token(token, &current, shell);
        }
        else if (token->type == TOKEN_DQUOTE)
        {
            process_double_quote_token(token, shell);
        }
        current = current->next;
    }
}
