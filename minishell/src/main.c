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

int	main(int ac, char **av, char **env)
{
	char		*input;
	t_list		*l_token;
	t_ast_node	*ast;

	(void) ac;
	(void) av;
	(void) env;
	l_token = NULL;
	ast = NULL;
	while (1)
	{
		input = get_input();
		if (!input)
			break;
		l_token = lexer(input);
		if (!l_token)
		{
			printf("Error: Lexer failed\n");
			cleanup(&l_token, &input, &ast);
			continue;
		}
		printer_token(l_token);  // Print tokens for debugging
		if (!ft_strncmp(token_content(l_token)->token, "exit", 5))
		{
			cleanup(&l_token, &input, &ast);
			exit(0);
		}
		ast= parse_pipe(&l_token);
		if (!ast)
		{
			printf("Error: Lexer failed\n");
			cleanup(&l_token, &input, &ast);
			continue;
		}
		cleanup(&l_token, &input, &ast);
	}
	cleanup(&l_token, &input, &ast);
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
