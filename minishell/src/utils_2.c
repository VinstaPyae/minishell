#include "minishell.h"

char	**trim_cmd(char **cmd_arg)
{
	char	**cmd;
	int	i;

	i = 0;
	if (!cmd_arg || !cmd_arg[0])
		return (NULL);
	while (cmd_arg[i])
	{
		i++;
	}
	cmd = malloc((i + 1) * sizeof(char *));
	if (!cmd)
		return (NULL);
	i = 0;
	while (cmd_arg[i])
	{
		cmd[i] = ft_strtrim(cmd_arg[i], " ");
		i++;
	}
	cmd[i] = NULL;
	return (cmd);
}
