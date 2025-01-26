#include "minishell.h"

int	n_option_checked(const char *str)
{
	int	i;

	if (str[0] != '-')
		return (0);
	i = 1;
	while (str[i] == 'n')
		i++;
	return (i > 1 && str[i] == '\0');
}

//echo builtin
int	exe_echo(t_ast_node **node)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	if (!(*node) || !(*node)->cmd_arg)
		return 1;
    printf("Executing echo with arguments:\n");
	if ((*node)->cmd_arg[i] && n_option_checked((*node)->cmd_arg[i]))
	{
		newline = 0;
		i++;
	}
	while ((*node)->cmd_arg[i])
	{
		printf("%s", (*node)->cmd_arg[i]);
		if ((*node)->cmd_arg[i + 1])
			printf(" ");
		i++;
	}
	if (newline)
		printf("\n");
	return (0);
}