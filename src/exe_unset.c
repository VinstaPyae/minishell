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

