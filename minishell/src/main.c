#include "minishell.h"

void	cleanup(t_list **l_token, char **input)
{
	if (*input)
	{
		free(*input);
		*input = NULL;
	}
	if (*l_token)
	{
		ft_lstclear(l_token, c_token_destroy);
		*l_token = NULL;
	}
	rl_clear_history();
	rl_free_line_state();
	rl_cleanup_after_signal();
	rl_deprep_terminal();
}

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
		if (l_token != NULL)
		{
			printer_token(l_token);
			if (!(ft_strncmp(token_content(l_token)->token, "exit", 5)))
			{
				cleanup(&l_token, &input);
				exit(0);
			}
		}
		cleanup(&l_token, &input);
	}
	cleanup(&l_token, &input);
	return (0);
}