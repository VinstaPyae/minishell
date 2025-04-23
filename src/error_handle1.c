#include "minishell.h"


void free_redir(void *redir)
{
	t_redir *r = (t_redir *)redir;
	if (r)
	{
		if (r->file)
			free(r->file);  // Free filename if dynamically allocated
		free(r);
	}
}


void	free_arg(char **str)
{
	int	i;

	if (!str) // Check if str is NULL before accessing it
		return;
	i = 0;
	while (str[i])
	{
		free(str[i]);
        str[i] = NULL;
		i++;
	}
	free(str);
    str = NULL; // Avoid dangling pointer
}

void	remove_node(t_list **head, t_list *node_to_remove, void (*del)(void *))
{
    t_list *current = *head;
    t_list *prev = NULL;

    while (current)
    {
        if (current == node_to_remove)
        {
            if (prev)
                prev->next = current->next;
            else
                *head = current->next;
            ft_lstdelone(current, del);
            return;
        }
        prev = current;
        current = current->next;
    }
}

t_list	*destroy_token_node(t_list *node)
{
	t_list	*next;
	t_token	*content;

	if (!node)
		return (NULL);
	next = node->next;
	content = (t_token *)node->content;
	c_token_destroy(content);
	free(node);
	return (next);
}