#include "minishell.h"

static void update_env_var(t_env **envp, char *var_name)
{
    t_env *current;
    t_env *prev;
    char *trimmed_v;

    current = *envp;
    prev = NULL;
    trimmed_v = ft_strtrim(var_name, " \n\r\t");
    if (!trimmed_v)
        return;
    while (current)
    {
        if (ft_strcmp(current->key, trimmed_v) == 0)
        {
		if (prev)
			prev->next = current->next;
		else
			*envp = current->next;
		(free(current->key),	free(current->value));
            	(free(current),	free(trimmed_v));
            	return;
        }
        prev = current;
        current = current->next;
    }
    free(trimmed_v);
}

int exe_unset(t_minishell **shell)
{
    int i;

    i = 1;
    if (!shell || !*shell || !(*shell)->ast || !(*shell)->ast->cmd_arg[1])
        return (1);
    while ((*shell)->ast->cmd_arg[i])
    {
        update_env_var(&(*shell)->envp, (*shell)->ast->cmd_arg[i]);
        i++;
    }
    return (0);
}
void handle_first_split_value(t_token *token, char **split_value, int i, int space1)
{
    if (ft_strcmp(split_value[i], "") == 0)
    {
        update_token_with_expansion(token, split_value[i], 0);
    }
    else if (split_value[i + 1])
    {
        update_token_with_expansion(token, split_value[i], 1);
    }
    else
    {
        update_token_with_expansion(token, split_value[i], space1);
    }
}

t_list *create_and_insert_node(t_list *insert_pos, char *value, int space)
{
    t_list *new_node;

    new_node = create_token(ft_strdup(value), TOKEN_WD, space);
    if (!new_node)
        return NULL;

    new_node->next = insert_pos->next;
    insert_pos->next = new_node;
    return new_node;
}

t_list *handle_remaining_split_values(t_list *insert_pos, char **split_value, int i, int space1)
{
    t_list *new_node;
    int space;

    space = 0;
    if (split_value[i + 1])
        space = 1;
    else
        space = space1;

    new_node = create_and_insert_node(insert_pos, split_value[i], space);
    if (!new_node)
        return NULL;

    return new_node;
}
