#include "minishell.h"

t_ast_node *create_command_node(void)
{
	t_ast_node *cmd_node;

	cmd_node = create_node(NODE_COMMAND);
	if (!cmd_node)
		return NULL;
	return cmd_node;
}

int count_parse_arguments(t_list *tokens)
{
	int count;

	count = 1; // Start at 1 for the first argument
	while (tokens != NULL && is_word_token(token_content(tokens)->type) &&
			token_content(tokens)->type != TOKEN_PIPE)
	{
		if (token_content(tokens)->space > 0 && tokens->next != NULL)
			count++; // Count separate arguments
		tokens = tokens->next;
	}
	return count;
}

char *join_tokens(t_list **tmp_list, char *arg)
{
	char *tmp;

	while ((*tmp_list)->next != NULL && token_content(*tmp_list)->space == 0 &&
			is_word_token(token_content((*tmp_list)->next)->type))
	{
		tmp = ft_strjoin(arg, token_content((*tmp_list)->next)->token);
		if (!tmp)
		{
			free(arg);
			return NULL;
		}
		free(arg);
		arg = tmp;
		*tmp_list = (*tmp_list)->next;
	}
	return arg;
}

int validate_redirection(t_list *tmp_list)
{
	int result;

	result = (tmp_list != NULL && is_word_token(((t_token *)tmp_list->content)->type) &&
			  ((t_token *)tmp_list->content)->type != TOKEN_PIPE);
	return result;
}

t_list *process_redirection(t_list **redir, t_token *current_token, t_list *tmp_list)
{
	t_list *new_redir;

	new_redir = create_redir(((t_token *)tmp_list->content)->token, current_token->type);
	if (!new_redir)
	{
		ft_lstclear(redir, free); // Clear the redirection list on failure
		return NULL;
	}
	ft_lstadd_back(redir, new_redir); // Add the new redirection to the list
	return tmp_list->next;           // Advance to the next token
}
