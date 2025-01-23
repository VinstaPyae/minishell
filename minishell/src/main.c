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

void print_redir(t_list *redir)
{
    t_list *current = redir;
    while (current != NULL)
    {
        t_redir *r = (t_redir *)current->content;
        printf("Redir Type: %d, File: %s\n", r->type, r->file);
        current = current->next;
    }
}

int	main(int ac, char **av, char **env)
{
	char		*input;
	t_list		*l_token;

	(void) ac;
	(void) av;
	(void) env;
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
			cleanup(&l_token, &input);
			continue;
		}
		printer_token(l_token);  // Print tokens for debugging
		char **cmd = get_cmd(&l_token);
		t_list **redir_list = get_redir(&l_token);
		if (redir_list != NULL)
		{
			print_redir(*redir_list);
			// Free the redir list after use
			ft_lstclear(redir_list, free);
			free(redir_list);
		}
		// int i = 0;
		// while (cmd[i])
		// 	printf("%s\n", cmd[i++]); 
		// if (!ft_strncmp(token_content(l_token)->token, "exit", 5))
		// {
		// 	cleanup(&l_token, &input);
		// 	exit(0);
		// }
		// cleanup(&l_token, &input);
	}
	// cleanup(&l_token, &input);
	return (0);
}
