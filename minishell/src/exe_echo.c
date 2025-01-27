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
int	exe_echo(t_ast_node **node)
{
	int	i;
	int	newline;

	i = 0;
	newline = 1;
	// Check if node or cmd_arg is NULL
	if (!(*node) || !(*node)->cmd)
		return 1;

	// If cmd_arg exists and has the -n option
	if ((*node)->cmd_arg && (*node)->cmd_arg[0] && n_option_checked((*node)->cmd_arg[0]))
	{
		newline = 0;
		i++;
	}
	// Print arguments if they exist
	if ((*node)->cmd_arg)
	{
		while ((*node)->cmd_arg[i])
		{
			printf("%s", (*node)->cmd_arg[i]);
			i++;
		}
	}

	
	if (newline)
		printf("\n");
	printf("Executing echo with arguments:\n");
	return (0);
}
