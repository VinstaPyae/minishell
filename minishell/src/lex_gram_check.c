#include "minishell.h"

int	check_pipe_grammar(t_list *l_token)
{
    t_token *current;
    t_token *next;

    if (!l_token)
        return (1); 

    current = l_token->content;
    if (current->type == TOKEN_PIPE)
    {
        printf("Syntax error: Pipe '|' cannot appear at the start of the input\n");
        return (1);
    }

    // Iterate through the tokens to check for invalid pipe positions
    while (l_token)
    {
        current = l_token->content;
        next = NULL;
        if (l_token->next)
            next = l_token->next->content;


        // Check if the current token is a pipe
        if (current->type == TOKEN_PIPE)
        {
            // Check if the pipe is at the end
            if (!next)
            {
                printf("Syntax error: Pipe '|' cannot appear at the end of the input\n");
                return (1);
            }

            // Check if the next token is invalid
            if (next->type == TOKEN_PIPE || next->type == TOKEN_APPEND || next->type == TOKEN_REDIRECT_IN ||
                next->type == TOKEN_REDIRECT_OUT || next->type == TOKEN_HDC)
            {
                printf("Syntax error: Invalid token '%s' after pipe '|'\n", next->token);
                return (1);
            }
        }

        l_token = l_token->next;
    }

    return (0); // Valid input
}

int	lexer_syntax_check(t_list *l_token)
{
	if (check_pipe_grammar(l_token) == 1)
		return (1);
	return (0);
}