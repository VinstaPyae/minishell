#include "minishell.h"

// Helper function to find and remove a specific environment variable
static void update_env_var(t_env **envp, char *var_name)
{
    t_env *current;
    t_env *prev;

    current = *envp;
    prev = NULL;

    printf("Searching for variable: %s\n", var_name); // Debug print
    while (current)
    {
        printf("Current key: %s\n", current->key);
        printf("Comparing %s (len: %zu) with %s (len: %zu)\n", current->key, strlen(current->key), var_name, strlen(var_name));

        if (ft_strcmp(current->key, ft_strtrim(var_name," \n\r\t")) == 0)
        {
            printf("Found variable: %s=%s\n", current->key, current->value); // Debug print
            if (prev)
                prev->next = current->next;
            else
                *envp = current->next; // Update head if the first node is removed
            free(current->key);
            free(current->value);
            free(current);
            printf("Variable removed: %s\n", var_name); // Debug print
            return; // Exit after removing the node
        }
        prev = current;
        current = current->next;
    }
    printf("Variable not found: %s\n", var_name); // Debug if variable is not found
}

int exe_unset(t_minishell **shell)
{
    int i;

    if (!shell || !*shell || !(*shell)->ast || !(*shell)->ast->cmd_arg || !(*shell)->ast->cmd_arg[0])
    {
        print_error("exe_unset", __FILE__, __LINE__, "invalid arguments");
        return (1);
    }

    printf("Arguments passed to unset:\n");
    i = 0; // Start from the first argument (cmd_arg[0] is the first argument, not the command name)
    while ((*shell)->ast->cmd_arg[i])
    {
        printf("cmd_arg[%d] = %s\n", i, (*shell)->ast->cmd_arg[i]); // Debug print
        update_env_var(&(*shell)->envp, (*shell)->ast->cmd_arg[i]);
        i++;
    }
    printf("Environment after unset:\n");
    print_env((*shell)->envp);

    return (0);
}

