#include "minishell.h"

t_token *create_token(t_token_type type, char *value)
{
    t_token *token = malloc(sizeof(t_token));
    if (!token)
        return NULL;
    token->type = type;
    token->value = value;
    return token;
}

t_list *create_list_node(t_token *token)
{
    t_list *node = malloc(sizeof(t_list));
    if (!node)
        return NULL;
    node->token = token;
    node->next = NULL;
    return node;
}

void add_to_list(t_list **list, t_list *new_node)
{
    if (!list || !new_node)
        return;

    if (!*list)
    {
        *list = new_node;
        return;
    }

    t_list *current = *list;
    while (current->next)
        current = current->next;
    current->next = new_node;
}

void free_list(t_list *list)
{
    while (list)
    {
        t_list *next = list->next;
        free(list->token->value);
        free(list->token);
        free(list);
        list = next;
    }
}