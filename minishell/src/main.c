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
	ast = NULL;
	l_token = NULL;
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
		ast = parse(&l_token);
		if (!ast)
		{
			printf("Error: Parsing failed\n");
			cleanup(&l_token, &input, &ast);
			continue;
		}
		// Print or execute the AST
		print_ast_tree(ast);  // Assuming print_ast_tree prints the AST
		// execute_ast(ast);  // Assuming execute_ast executes the AST
		cleanup(&l_token, &input, &ast);
	}
	cleanup(&l_token, &input, &ast);
	return (0);
}