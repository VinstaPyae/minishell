#include "minishell.h"

char	*get_input(void)
{
	char	*input;

	input = readline("minishell$>");
	if (!input)
		return (NULL);
	add_history(input);
	return (input);
}

int	main(int ac, char **av, char **env)
{
	char	*input;
	t_list	*l_token;

	(void) ac;
	(void) av;
	(void) env;
	while (1)
	{
		input = get_input();
		if (!input)
			break;
		l_token = lexer(input);
		t_token *token = l_token->content;
		printf("Token: Type = %d, Value = %s \n", token->type, token->token);
	}
	return (0);
}