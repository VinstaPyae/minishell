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