#include "minishell.h"

char *get_input(t_minishell *shell)
{
    char *input;

    
    input = readline("minishell$> ");
    if (!input)
    {
        handle_eof(shell);
        return NULL;
    }
    if (*input)
        add_history(input);
    return input;
}

int process_tokens_and_ast(t_minishell *shell)
{
    shell->l_token = lexer(shell);
    if (!shell->l_token)
    {
        shell->exit_status = 1;
        cleanup(&shell);
        return 0; // Skip further processing
    }
    expand_tokens(shell);
    printer_token(shell->l_token);
    shell->ast = parse_pipe(shell->l_token);
    if (!shell->ast)
    {
        shell->exit_status = 1;
        printf("Error: AST creation failed\n");
        cleanup(&shell);
        return 0; // Skip further processing
    }
    return 1; // Continue processing
}

// Function to handle heredocs and execute AST
int handle_heredocs_and_execute(t_minishell *shell)
{
    if (process_heredocs(shell->ast, shell) == -1)
    {
        if (g_signal_status == 130)
            shell->exit_status = 130;
        else
            shell->exit_status = 1;
        g_signal_status = 0; // Reset signal status
        close_heredoc_fds(shell->ast);
        cleanup(&shell);
        return 0; // Skip execution
    }
    shell->exit_status = execute_ast(shell->ast, shell);
    close_heredoc_fds(shell->ast);
    return 1; // Execution completed
}

// Main loop for minishell
void minishell_loop(t_minishell *shell)
{
    while (1)
    {
        if (!handle_input_and_signals(shell))
            continue;
        if (!process_tokens_and_ast(shell))
            continue;
        if (!handle_heredocs_and_execute(shell))
            continue;
    }
}

// Main function
int main(int ac, char **av, char **env)
{
    t_minishell *shell;

    (void)ac;
    (void)av;
    g_signal_status = 0;
    setup_signal_handlers();

    shell = initialize_shell(env);
    if (!shell)
        return 1;
    minishell_loop(shell);

    cleanup(&shell);
    if (shell->envp)
        free_env_list(shell->envp);
    free(shell);
    rl_clear_history();
    return 0;
}

// int main(int ac, char **av, char **env)
// {
//     t_minishell *shell;
//     t_env *envp;

//     (void)ac;
//     (void)av;
   
//     g_signal_status = 0;
//     setup_signal_handlers();

//     envp = init_env(env);

//     if (!envp)
//     {
//         printf("Error: Failed to initialize environment\n");
//         return (1);
//     }
//     shell = create_minshell(envp);
//     printf("Calling minishell!\n");

//     while (1)
//     {
//         // shell = create_minshell(envp);
//         // if (!shell)
//         // {
//         //     printf("Error: Failed to initialize minishell\n");
//         //     cleanup(&shell);
//         //     // free_env_list(shell->envp);
//         //     return (1);
//         // }
//          // Add this line
//         //printf("Signal status: %d\n", g_signal_status);
//         shell->input = get_input(shell);
//         // Check signal status after input
//         if (g_signal_status == 130)
//         {
//             shell->exit_status = 130;
//             g_signal_status = 0; // Reset after handling
//         }
//         if (!shell->input || ft_strlen(shell->input) == 0)
//         {
//             free(shell->input);
//             shell->input = NULL;
//             continue;
//         }
//         shell->l_token = lexer(shell);
//         if (!shell->l_token)
//         {
//             // printf("Error: Lexer failed\n");
//             shell->exit_status = 1;
//             cleanup(&shell);
//             continue;
//         }
//         // printer_token(shell->l_token);
//         expand_tokens(shell);
//         // printer_token(shell->l_token);
//         // if (check_expand_tokens(shell->l_token) == -1)
//         // {
//         //     cmd_error_msg(CMD_NOT_FOUND, "''", shell);
//         //     cleanup(&shell);
//         //     continue;
//         // }
//         printer_token(shell->l_token);
//         shell->ast = parse_pipe(shell->l_token);
//         if (!shell->ast)
//         {
//             shell->exit_status = 1;
//             printf("Error: ast failed\n");
//             cleanup(&shell);
//             continue;
//         }
//         print_ast_node(shell->ast); // Print AST for debugging
//         if (process_heredocs(shell->ast, shell) == -1)
//         {
//             // Check if the heredoc was interrupted by SIGINT
//             if (g_signal_status == 130)
//                 shell->exit_status = 130;
//             else
//                 shell->exit_status = 1;
//             g_signal_status = 0; // Reset signal status
//             close_heredoc_fds(shell->ast);
//             cleanup(&shell);
//             continue; // Continue to next loop iteration instead of exiting
//         }
//         shell->exit_status = execute_ast(shell->ast, shell); // This should call exe_exit for the "exit" command
//         close_heredoc_fds(shell->ast);
//         //printf("Exit status: %d\n", shell->exit_status);
//         // print_ast_node(shell->ast); // Print AST for debugging
//         //printf("Hello from main after execute_ast\n");
        

//     }

//     printf("Main before cleanup\n");
//     cleanup(&shell); // Final cleanup
//     if (shell->envp)
//         free_env_list(shell->envp);
//     free(shell);
//     rl_clear_history(); // Clear readline history
//     printf("Main after cleanup\n");
//     return (0);
// }

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
