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
	char *value;

	if (!var)
		return (NULL);
	if (var[1] == '\0')
		return (ft_strdup("$"));

	if (var[1] == '?' && var[2] == '\0') // Handle $?
		return (ft_itoa(shell->exit_status));

	value = get_env_value(shell->envp, &var[1]);
	if (value)
		return (ft_strdup(value));
	
	return (ft_strdup(""));
}

char *expand_double_quotes(char *input, t_minishell *shell)
{
    char *result = ft_strdup(""); // Start with an empty string
    char *temp;
    int i = 0;

    while (input[i])
    {
        if (input[i] == '$') // Found variable expansion
        {
            int j = i + 1;
            
            // Handle special case: "$?"
            if (input[j] == '?')
                j++;

            // Extract variable name
            while (ft_isalnum(input[j]) || input[j] == '_')
                j++;

            char *var_name = ft_substr(input, i, j - i); // Extract "$VAR"
            char *expanded = expand_variable(var_name, shell); // Expand it
            free(var_name);

            // Append the expanded variable to the result
            temp = ft_strjoin(result, expanded);
            free(result);
            free(expanded);
            result = temp;

            i = j; // Move past the variable
        }
        else
        {
            // Append normal character
            char char_str[2] = {input[i], '\0'}; // Convert char to string
            temp = ft_strjoin(result, char_str);
            free(result);
            result = temp;

            i++;
        }
    }
    return result;
}



void expand_tokens(t_minishell *shell)
{
    t_list *current = shell->l_token;
    t_list *before_c = shell->l_token;
    while (current)
    {
        t_token *token = (t_token *)current->content;
        // Handle variables inside double quotes or standalone
        if (token->type == TOKEN_VARIABLE)
        {
		//printf("before token: %s\n", ((t_token *)before_c->content)->token);
		char *expanded_value = expand_variable(token->token, shell);
		free(token->token);
		token->token = expanded_value;
		if ((!expanded_value[0]) && ((t_token *)before_c->content)->space > 0)
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
	if (token->type == TOKEN_DQUOTE)
	{
		char *d_qvalue = expand_double_quotes(token->token, shell);
		free(token->token);
		token->token = d_qvalue;
	}
	if (current != before_c)
		before_c = before_c->next;
        current = current->next;
    }
}

