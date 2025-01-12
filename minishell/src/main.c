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
	t_token *token;

	(void) ac;
	(void) av;
	(void) env;
	while (1)
	{
		input = get_input();
		if (!input)
			break;
		l_token = lexer(input);
		while (l_token != NULL)
		{
			token = l_token->content;
			printf("Token: Type = %d, Value = %s \n", token->type, token->token);
			l_token = l_token->next;
			free(token);
		}
	}
	free_token_list(&l_token);
	//free_token_list(&temp);
	free(input);
	return (0);
}