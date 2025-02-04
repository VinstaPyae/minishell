#include "minishell.h"

char	*get_input(void)
{
	char	*input;

	input = readline("minishell$>");
	if (!input)
		return (NULL);
	if (*input)
		add_history(input);
	return (input);
}

t_minishell	*create_minshell(t_env *envp)
{
	t_minishell	*shell;

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
	return (shell);
}

int main(int ac, char **av, char **env)
{
    t_minishell *shell;
    t_env   *envp;

    (void)ac;
    (void)av;
	shell = NULL;
    envp = init_env(env);
    if (!envp)
    {
        printf("Error: Failed to initialize environment\n");
        return (1);
    }
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
	// printer_token(shell->l_token);
        shell->ast = parse_pipe(&shell->l_token);
        if (!shell->ast)
        {
            printf("Error: ast failed\n");
            cleanup(&shell);
            continue;
        }
        execute_ast(&shell); // This should call exe_exit for the "exit" command
        cleanup(&shell);     // Clean up after each iteration
    }
    cleanup(&shell);         // Final cleanup
    free_env(envp);
    rl_clear_history();      // Clear readline history
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
