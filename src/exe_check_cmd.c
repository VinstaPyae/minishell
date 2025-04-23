/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exe_check_cmd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 01:55:26 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 01:55:27 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	calculate_lengths(const char **strings, int *tmp)
{
	int	cpylen;

	init_tmp(tmp);
	if (!strings[0] && !strings[1] && !strings[2] && !strings[3])
		return (0);
	if (strings[0])
		tmp[1] = ft_strlen(strings[0]);
	if (strings[1])
		tmp[2] = ft_strlen(strings[1]);
	if (strings[2])
		tmp[3] = ft_strlen(strings[2]);
	if (strings[3])
		tmp[4] = ft_strlen(strings[3]);
	cpylen = tmp[1] + tmp[2] + tmp[3] + tmp[4];
	return (cpylen);
}

static void	copy_strings(char *result, const char **strings, int *tmp)
{
	if (strings[0])
		ft_memcpy(result, strings[0], tmp[1]);
	if (strings[1])
		ft_memcpy(result + tmp[1], strings[1], tmp[2]);
	if (strings[2])
		ft_memcpy(result + tmp[1] + tmp[2], strings[2], tmp[3]);
	if (strings[3])
		ft_memcpy(result + tmp[1] + tmp[2] + tmp[3], strings[3], tmp[4]);
}

char	*ft_strnc(char *s1, const char *s2, const char *s3, const char *s4)
{
	char		*result;
	const char	*strings[4];
	int			tmp[5];
	int			cpylen;

	strings[0] = s1;
	strings[1] = s2;
	strings[2] = s3;
	strings[3] = s4;
	cpylen = calculate_lengths(strings, tmp);
	if (cpylen == 0)
		return (NULL);
	result = (char *)malloc((cpylen + 1) * (sizeof(char)));
	if (!result)
		return (NULL);
	copy_strings(result, strings, tmp);
	result[cpylen] = 0;
	return (result);
}

static	t_error_cmd	check_path(char *cmd, t_minishell *shell)
{
	struct stat		sb;
	char			*f_path;
	int				i;

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

t_error_cmd	search_cmd_path(char *cmd, t_minishell *shell)
{
	struct stat	sb;
	t_env		*env;

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
