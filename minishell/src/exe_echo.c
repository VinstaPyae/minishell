#include "minishell.h"

int	n_option_checked(const char *str)
{
	int	i;
	
	if (str[0] != '-')
		return (0);
	i = 1;
	while (str[i] == 'n')
		i++;
	return (i > 1);
}

//echo builtin
int	exe_echo(t_minishell **shell)
{
	int	i;
	int	newline;

	i = 0;
	newline = 1;
	// Check if node or cmd_arg is NULL
	if (!(*shell)->ast || !(*shell)->ast->cmd)
		return 1;

	// If cmd_arg exists and has the -n option
	if ((*shell)->ast->cmd_arg && (*shell)->ast->cmd_arg[0] && n_option_checked((*shell)->ast->cmd_arg[0]))
	{
		newline = 0;
		i++;
	}
	// Print arguments if they exist
	if ((*shell)->ast->cmd_arg)
	{
		while ((*shell)->ast->cmd_arg[i])
		{
			printf("%s", (*shell)->ast->cmd_arg[i]);
			i++;
		}
	}

	
	if (newline)
		printf("\n");
	//printf("Executing echo with arguments:\n");
	return (0);
}

int	exe_exit(t_minishell **shell)
{
	if ((*shell))
	{
		cleanup(shell);
	}
	rl_clear_history();
	exit(0);
}