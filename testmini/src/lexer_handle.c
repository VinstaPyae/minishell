#include "minishell.h"

void handle_redirect(char *input, int *i, t_list **list)
{
    if (input[*i] == '<')
    {
        if (input[*i + 1] == '<') // Heredoc ("<<")
        {
            char *redirect = strndup(&input[*i], 2);
            t_token *token = create_token(TOKEN_HDC, redirect);
            t_list *node = create_list_node(token);
            add_to_list(list, node);
            *i += 2;
        }
        else // Input redirection ("<")
        {
            char *redirect = strndup(&input[*i], 1);
            t_token *token = create_token(TOKEN_REDIRECT_IN, redirect);
            t_list *node = create_list_node(token);
            add_to_list(list, node);
            (*i)++;
        }
    }
    else if (input[*i] == '>')
    {
        if (input[*i + 1] == '>') // Append output redirection (">>")
        {
            char *redirect = strndup(&input[*i], 2);
            t_token *token = create_token(TOKEN_REDIRECT_OUT, redirect);
            t_list *node = create_list_node(token);
            add_to_list(list, node);
            *i += 2;
        }
        else // Output redirection (">")
        {
            char *redirect = strndup(&input[*i], 1);
            t_token *token = create_token(TOKEN_REDIRECT_OUT, redirect);
            t_list *node = create_list_node(token);
            add_to_list(list, node);
            (*i)++;
        }
    }
}

void handle_command(char *input, int *i, t_list **list)
{
    int start = *i;
    while (input[*i] && !isspace(input[*i]) && input[*i] != '|' && input[*i] != '>' && input[*i] != '<')
        (*i)++;
    char *cmd = strndup(&input[start], *i - start);
    t_token *token = create_token(TOKEN_CMD, cmd);
    t_list *node = create_list_node(token);
    add_to_list(list, node);
}

void handle_pipe(char *input, int *i, t_list **list)
{
	(void)input;
    t_token *token = create_token(TOKEN_PIPE, strdup("|"));
    t_list *node = create_list_node(token);
    add_to_list(list, node);
    (*i)++;
}

void handle_bracket(char *input, int *i, t_list **list)
{
    char *bracket = strndup(&input[*i], 1);
    t_token *token = create_token(TOKEN_BRACKET, bracket);
    t_list *node = create_list_node(token);
    add_to_list(list, node);
    (*i)++;
}

void handle_quote(char *input, int *i, t_list **list)
{
    //char quote = input[*i];
    t_token *token = create_token(TOKEN_QUOTE, strndup(&input[*i], 1));
    t_list *node = create_list_node(token);
    add_to_list(list, node);
    (*i)++;
}

void handle_word(char *input, int *i, t_list **list)
{
    int start = *i;
    while (input[*i] && !isspace(input[*i]) && input[*i] != '|' && input[*i] != '>' && input[*i] != '<')
        (*i)++;
    char *word = strndup(&input[start], *i - start);
    t_token *token = create_token(TOKEN_WORD, word);
    t_list *node = create_list_node(token);
    add_to_list(list, node);
}