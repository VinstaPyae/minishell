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

    printf("Searching for variable: %s\n", trimmed_v); // Use trimmed_v here for clarity

    while (current)
    {
        printf("Current key: %s\n", current->key);
        printf("Comparing %s (len: %zu) with %s (len: %zu)\n", current->key, strlen(current->key), trimmed_v, strlen(trimmed_v));

        if (ft_strcmp(current->key, trimmed_v) == 0)
        {
            printf("Found variable: %s=%s\n", current->key, current->value);
            if (prev)
                prev->next = current->next;
            else
                *envp = current->next;

            free(current->key);
            free(current->value);
            free(current);
            printf("Variable removed: %s\n", trimmed_v);
            free(trimmed_v);  // Free after use
            return;
        }
        prev = current;
        current = current->next;
    }

    printf("Variable not found: %s\n", trimmed_v);
    free(trimmed_v);  // Free even if not found
}


int exe_unset(t_minishell **shell)
{
    int i;

    if (!shell || !*shell || !(*shell)->ast || !(*shell)->ast->cmd_arg[1])
    {
        print_error("exe_unset", __FILE__, __LINE__, "invalid arguments");
        return (1);
    }

    i = 1; // Start from the first argument (cmd_arg[0] is the first argument, not the command name)
    while ((*shell)->ast->cmd_arg[i])
    {
        update_env_var(&(*shell)->envp, (*shell)->ast->cmd_arg[i]);
        i++;
    }

    return (0);
}
