#include "minishell.h"

// char    *get_olden(char *key, t_minishell **shell)
// {
//     t_env *env = (*shell)->envp;

//     while (env)
//     {
//         if (ft_strcmp(env->key, key) == 0)
//             return (ft_strdup(env->value));
//         env = env->next;
//     }
//     return (NULL);
// }

// void update_shlvl(t_env *envp)
// {
//     char *shlvl_str = getenv("SHLVL");
//     int shlvl;

//     if (shlvl_str == NULL)
//         shlvl = 1;
//     else
//         shlvl = atoi(shlvl_str) + 1;

//     char *new_shlvl = ft_itoa(shlvl);
//     if (new_shlvl == NULL)
//     {
//         perror("ft_itoa");
//         return;
//     }
//     replace_or_add_env_var("SHELVL", new_shlvl, envp);
//     free(new_shlvl);
// }

char *get_input(void)
{
    char *input;

    input = readline("minishell$>");
    if (!input)
    {
        printf("exit\n");
        exit(0);
    }
    if (*input)
        add_history(input);
    return (input);
}

t_minishell *create_minshell(t_env *envp)
{
    t_minishell *shell;

    shell = malloc(sizeof(t_minishell));
    if (!shell)
        return (NULL);
    shell->ast = NULL;
    shell->input = NULL;
    shell->l_token = NULL;
    shell->envp = envp;
    if (!shell->envp)
    {
        free(shell);
        return (NULL);
    }
    shell->exit_status = 0;
    return (shell);
}

int main(int ac, char **av, char **env)
{
    t_minishell *shell;
    t_env *envp;

    (void)ac;
    (void)av;
    setup_signal_handlers(); // Add this line
    shell = NULL;
    envp = init_env(env);
    if (!envp)
    {
        printf("Error: Failed to initialize environment\n");
        return (1);
    }
    printf("Calling minishell!\n");
    while (1)
    {
        shell = create_minshell(envp);
        if (!shell)
        {
            printf("Error: Failed to initialize minishell\n");
            cleanup(&shell);
            free_env(envp);
            return (1);
        }
        shell->input = get_input();
        if (!shell->input)
            break;
        shell->l_token = lexer(shell->input);
        if (!shell->l_token)
        {
            printf("Error: Lexer failed\n");
            cleanup(&shell);
            continue;
        }
        printer_token(shell->l_token);
        expand_tokens(shell);
        shell->ast = parse_pipe(shell->l_token);
        if (!shell->ast)
        {
            printf("Error: ast failed\n");
            cleanup(&shell);
            continue;
        }
        execute_ast(&shell); // This should call exe_exit for the "exit" command
        cleanup(&shell);     // Clean up after each iteration
    }
    cleanup(&shell); // Final cleanup
    free_env(envp);
    rl_clear_history(); // Clear readline history
    return (0);
}

// int main(int ac, char **av, char **env)
// {
//     char        *input;
//     t_list      *l_token;
//     t_ast_node  *ast;

//     (void) ac;
//     (void) av;
//     (void) env;
//     l_token = NULL;
//     ast = NULL;

//     while (1)
//     {
//         input = get_input();
//         if (!input)
//             break;

//         l_token = lexer(input);
//         if (!l_token)
//         {
//             printf("Error: Lexer failed\n");
//             cleanup(&l_token, &input, &ast);
//             continue;
//         }
// 	if (!ft_strncmp(token_content(l_token)->token, "exit", 5))
//         {
//             cleanup(&l_token, &input, &ast);
//             exit(0);
//         }

//         printer_token(l_token);  // Print tokens for debugging

//         ast = parse_pipe(&l_token);
//         if (!ast)
//         {
//             printf("Error: Parser failed\n");
//             cleanup(&l_token, &input, &ast);
//             continue;
//         }
//         // Use the AST for execution or further processing
//         // execute_ast(ast);
//         cleanup(&l_token, &input, &ast);
//     }

//     cleanup(&l_token, &input, &ast);
//     return 0;
// }

