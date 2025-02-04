#include "minishell.h"

t_list *lexer(char *input)
{
	t_list	*token;

	token = get_token_list(input);
	if (!token)
		return (NULL);
	t_token *eof_token = create_token(TOKEN_EOF, NULL);
	t_list *eof_node = create_list_node(eof_token);
	add_to_list(&token, eof_node);
	return (token);
}

t_list	*get_token_list(char *input)
{
	t_list *list;
	t_token *token;
	
	list = NULL;
	int i = 0;
	token = (t_token *)list->content;

	while (input[i])
	{
		while (isspace(input[i]))
		i++;
		// Handle end of input
		if (input[i] == '\0')
		break;

		// Handle pipes
		if (input[i] == '|')
		{
			handle_pipe(input, &i, &list);
			continue;
		}

		// Handle redirections
		if (input[i] == '>' || input[i] == '<')
		{
			handle_redirect(input, &i, &list);
			continue;
		}

		// Handle brackets
		if (input[i] == '(' || input[i] == ')')
		{
			handle_bracket(input, &i, &list);
			continue;
		}

		// Handle quotes
		if (input[i] == '\'' || input[i] == '"')
		{
			handle_quote(input, &i, &list);
			continue;
		}

		// Handle commands (first word is treated as a command)
		if (!list || token->type == TOKEN_PIPE)
		{
			handle_command(input, &i, &list);
		}
		else
		{
			
		}
	}
	return (list);
}