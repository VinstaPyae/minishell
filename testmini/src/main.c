#include "minishell.h"

static char	*get_input(void)
{
	char	*input;

	input = readline("minishell$>");
	if (!input)
		return (NULL);
	add_history(input);
	return (input);
}

/* int	main(int ac, char **av, char **env)
{
	char	*input;
	t_list	*token;

	(void) ac;
	(void) av;
	(void) env;
	while (1)
	{
		input = get_input();
		if (!input)
			break;
		token = lexer(input);
		t_list *current = token;
		while (current)
		{
			printf("Token: Type = %d, Value = %s\n", current->content->type , current->content->value);
			current = current->next;
		}
		// Free the list and tokens
		free_list(token);
		free(input);
	}
	return 0;
} */

int	main(int ac, char **av, char **env)
{
	char	*input;
	t_list	*token;

	(void) ac;
	(void) av;
	(void) env;
	while (1)
	{
		input = get_input();
		if (!input)
			break;
		token = lexer(input);
		t_list *current = token;
		while (current)
		{
			// Cast current->content to t_token* before accessing its fields
			t_token *content = (t_token *)current->content;
			printf("Token: Type = %d, Value = %s\n", content->type, content->token);
			current = current->next;
		}

		// Free the list and tokens
		free_list(token);
		free(input);
	}
	return 0;
}
