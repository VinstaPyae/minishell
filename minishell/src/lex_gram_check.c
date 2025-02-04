#include "minishell.h"

int check_pipe_grammar(t_list *l_token)
{
    t_token *current;
    t_token *next;

    if (!l_token)
        return (1); 
    current = l_token->content;
    if (current->type == TOKEN_PIPE)  
        return (printf("Syntax error: Pipe '|' cannot appear at the start of the input\n"),1);
    while (l_token)
    {
        current = l_token->content;
        next = NULL;
        if (l_token->next)
            next = l_token->next->content;
        if (current->type == TOKEN_PIPE)
        {
            if (!next)
                return (printf("Syntax error: Pipe '|' cannot appear at the end of the input\n"),1);
            if (next->type == TOKEN_PIPE || next->type == TOKEN_APPEND || next->type == TOKEN_REDIRECT_IN ||
                next->type == TOKEN_REDIRECT_OUT || next->type == TOKEN_HDC)
                return (printf("Syntax error: Invalid token '%s' after pipe '|'\n", next->token), 1);
        }
        l_token = l_token->next;
    }
    return (0);
}

int check_redirect_in_grammar(t_list *l_token)
{
    t_token *current;
    t_token *next;

    if (!l_token)
        return (1); 
    while (l_token)
    {
        current = l_token->content;
        next = NULL;
        if (l_token->next)
            next = l_token->next->content;
        if (current->type == TOKEN_REDIRECT_IN)
        {
            if (!next)
                return (printf("Syntax error: Input redirection '<' cannot appear at the end of the input\n"), 1);
            if (next->type != TOKEN_WD)
                return (printf("Syntax error: Invalid token '%s' after input redirection '<'\n", next->token), 1);
        }
        l_token = l_token->next;
    }
    return (0);
}

int check_redirect_out_grammar(t_list *l_token)
{
    t_token *current;
    t_token *next;

    if (!l_token)
        return (1); 
    while (l_token)
    {
        current = l_token->content;
        next = NULL;
        if (l_token->next)
            next = l_token->next->content;
        if (current->type == TOKEN_REDIRECT_OUT)
        {
            if (!next)
                return (printf("Syntax error: Output redirection '>' cannot appear at the end of the input\n"), 1);
            if (next->type != TOKEN_WD)
                return (printf("Syntax error: Invalid token '%s' after output redirection '<'\n", next->token), 1);
        }
        l_token = l_token->next;
    }
    return (0);
}

int check_append_grammar(t_list *l_token)
{
    t_token *current;
    t_token *next;

    if (!l_token)
        return (1); 
    while (l_token)
    {
        current = l_token->content;
        next = NULL;
        if (l_token->next)
            next = l_token->next->content;
        if (current->type == TOKEN_APPEND)
        {
            if (!next)
                return (printf("Syntax error: Append '>>' cannot appear at the end of the input\n"), 1);
            if (next->type != TOKEN_WD)
                return (printf("Syntax error: Invalid token '%s' after Append '>>'\n", next->token), 1);
        }
        l_token = l_token->next;
    }
    return (0);
}

int check_heredoc_grammar(t_list *l_token)
{
    t_token *current;
    t_token *next;

    if (!l_token)
        return (1); 
    while (l_token)
    {
        current = l_token->content;
        next = NULL;
        if (l_token->next)
            next = l_token->next->content;
        if (current->type == TOKEN_HDC)
        {
            if (!next)
                return (printf("Syntax error: Heredoc '<<' cannot appear at the end of the input\n"), 1);
            if (next->type != TOKEN_WD)
                return (printf("Syntax error: Invalid token '%s' after Heredoc '<<'\n", next->token), 1);
        }
        l_token = l_token->next;
    }
    return (0);
}

// int check_word_grammar(t_list *l_token)
// {
//     t_token *current;
//     t_token *prev = NULL;

//     if (!l_token)
//         return (1);
//     while (l_token)
//     {
//         current = l_token->content;
//         if (current->type == TOKEN_WD)
//         {
//             // Check invalid previous tokens (if any)
//             if (prev && prev->type != TOKEN_PIPE && prev->type != TOKEN_REDIRECT_IN &&
//                 prev->type != TOKEN_REDIRECT_OUT && prev->type != TOKEN_HDC &&
//                 prev->type != TOKEN_APPEND && prev->type != TOKEN_WD && )
//                 return (printf("Syntax error: Unexpected word '%s' after token '%s'\n", current->token, prev->token), 1);
//         }
//         prev = current;
//         l_token = l_token->next;
//     }
//     return (0);
// }

int	lexer_syntax_check(t_list *l_token)
{
	if (check_pipe_grammar(l_token) == 1)
		return (printf("Invalid grammar for pipe.\n"), 1);
	if (check_redirect_in_grammar(l_token) == 1)
		return (printf("Invalid grammar for input direction.\n"), 1);
	if (check_redirect_out_grammar(l_token) == 1)
		return (printf("Invalid grammar for output direction.\n"), 1);
	if (check_append_grammar(l_token) == 1) 
		return (printf("Invalid grammar for append.\n"), 1);
	if (check_heredoc_grammar(l_token) == 1) 
		return (printf("Invalid grammar for heredoc.\n"), 1);
	// if (check_word_grammar(l_token) == 1) 
	// 	return (printf("Invalid grammar for word.\n"), 1);
	return (0);
}
