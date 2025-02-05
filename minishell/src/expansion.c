#include "minishell.h"

char *get_env_value(t_env *env, char *key)
{
    while (env)
    {
        if (ft_strcmp(env->key, key) == 0)
            return (env->value); // Return the value if key matches
        env = env->next;
    }
    return (NULL); // Return NULL if key is not found
}


char *expand_variable(char *var, t_minishell *shell)
{
    if (!var)
        return (NULL);

    if (var[1] == '?' && var[2] == '\0') // Handle $?
        return (ft_itoa(shell->exit_status));

    char *value = get_env_value(shell->envp, &var[1]);
    if (value)
        return (ft_strdup(value));
    
    return (ft_strdup(""));
}


void expand_tokens(t_minishell *shell)
{
    t_list *current = shell->l_token;
    while (current)
    {
        t_token *token = (t_token *)current->content;

        // Skip expansion inside single quotes
        if (token->type == TOKEN_SQUOTE)
        {
            current = current->next;
            continue;
        }

        // Handle variables inside double quotes or standalone
        if (token->type == TOKEN_VARIABLE)
        {
            char *expanded_value = expand_variable(token->token, shell);
            free(token->token);
            token->token = expanded_value;
	    if (!expanded_value[0])
	    	token->space = 0;
            // Convert TOKEN_VARIABLE → TOKEN_WD after expansion
            if (token->type == TOKEN_VARIABLE)
                token->type = TOKEN_WD;
            // If expansion results in an empty string, remove token
        	// if (!expanded_value[0])
		// {
		// t_list *to_remove = current;
		// current = current->next;
		// printf("list remove!\n");
		// remove_node(&shell->l_token, to_remove, c_token_destroy);
		// continue;
		// }
        }
        current = current->next;
    }
}

