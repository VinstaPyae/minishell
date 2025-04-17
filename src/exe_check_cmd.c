#include "minishell.h"

void init_tmp(int *tmp)
{
	tmp[0] = 0;
	tmp[1] = 0;
	tmp[2] = 0;
	tmp[3] = 0;
	tmp[4] = 0;
}

char	*ft_strnc(char *s1, const char *s2, const char *s3, const char *s4)
{
	char		*result;
	int			cpylen;
	int		tmp[5];

	init_tmp(tmp);
	if (!s1 && !s2 && !s3 && !s4)
		return (NULL);
	if (s1)
		tmp[1] = ft_strlen(s1);
	if (s2)
		tmp[2] = ft_strlen(s2);
	if (s3)
		tmp[3] = ft_strlen(s3);
	if (s4)
		tmp[4] = ft_strlen(s4);
	cpylen = tmp[1] + tmp[2] + tmp[3] + tmp[4];
	result = (char *)malloc((cpylen + 1) * (sizeof(char)));
	if (!result)
		return (NULL);
	if (s1)
		ft_memcpy(result, s1, tmp[1]);
	if (s2)
		ft_memcpy(result + tmp[1], s2, tmp[2]);
	if (s3)
		ft_memcpy(result + tmp[1] + tmp[2], s3, tmp[3]);
	if (s4)
		ft_memcpy(result + tmp[1] + tmp[2] + tmp[3], s4, tmp[4]);
	result[cpylen] = 0;
	return (result);
}

static t_error_cmd check_path(char *cmd, t_minishell *shell)
{
	char *f_path;
	struct stat sb;
	int i;

	i = -1;
	while (shell->env_path[++i])
	{
		f_path = ft_strnc(shell->env_path[i], "/", cmd, NULL);
		if (!f_path)
			return (free_arg(shell->env_path), CMD_NOT_FOUND);
		if (stat(f_path, &sb) == 0 && !S_ISDIR(sb.st_mode)
			&& access(f_path, X_OK) == 0)
		{
			shell->path = f_path;
			free_arg(shell->env_path);
			return (OK_CMD);
		}
		free(f_path);
	}
	free_arg(shell->env_path);
	return (CMD_NOT_FOUND);
}

t_error_cmd search_cmd_path(char *cmd, t_minishell *shell)
{
	t_env *env;
	struct stat sb;

	//handle_child_signals();
	if (ft_strchr(cmd, '/'))
	{
			if (stat(cmd, &sb) != 0)
				return (CMD_NOT_FOUND);
			if (S_ISDIR(sb.st_mode))
				return (CMD_IS_DIR);
			if (access(cmd, X_OK) != 0)
				return (CMD_NO_PERM);
			shell->path = ft_strdup(cmd);
			return (OK_CMD);
	}
	env = search_env_list(shell->envp, "PATH");
	if (!env)
		return (CMD_NOT_FOUND);
	shell->env_path = ft_split(env->value, ':');
	if (!shell->env_path)
		return (CMD_NOT_FOUND);
	return (check_path(cmd, shell));
}