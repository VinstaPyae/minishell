/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exe_cd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 01:55:19 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 01:55:19 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_env	*replace_or_add_env_var(const char *name,
	const char *value, t_env *envp)
{
	t_env	*head;
	int		found;

	head = find_and_update_env_var(name, value, envp, &found);
	if (!found)
		head = add_new_env_var(name, value, head);
	return (head);
}

char	*get_oldpwd(char *key, t_minishell **shell)
{
	t_env	*env;

	env = (*shell)->envp;
	while (env)
	{
		if (ft_strcmp(env->key, key) == 0)
			return (ft_strdup(env->value));
		env = env->next;
	}
	return (NULL);
}

static void	update_env_vars(t_minishell **shell)
{
	char	cwd[1024];
	char	*oldpwd;

	oldpwd = get_oldpwd("PWD", shell);
	if (!oldpwd)
		oldpwd = ft_strdup("");
	if (oldpwd)
	{
		replace_or_add_env_var("OLDPWD", oldpwd, (*shell)->envp);
		free(oldpwd);
	}
	if (getcwd(cwd, sizeof(cwd)))
		replace_or_add_env_var("PWD", cwd, (*shell)->envp);
	else
		perror("pwd");
}

static char	*path_handle(t_ast_node *ast, t_minishell *shell)
{
	char	*dir;

	if (!ast->cmd_arg[1] || ft_strncmp(ast->cmd_arg[1], "~", 2) == 0)
	{
		dir = ft_getenv((shell)->envp, "HOME");
		if (!dir)
		{
			printf("minishell: cd: HOME not set\n");
			return (NULL);
		}
	}
	else if (ft_strncmp(ast->cmd_arg[1], "-", 2) == 0)
	{
		dir = ft_getenv((shell)->envp, "OLDPWD");
		if (!dir)
		{
			printf("minishell: cd: OLDPWD not set\n");
			return (NULL);
		}
		printf("%s\n", dir);
	}
	else
		dir = ast->cmd_arg[1];
	return (dir);
}

int	exe_cd(t_ast_node *ast, t_minishell *shell)
{
	char	*path;
	char	*curr_dir;

	if (!ast)
		return (1);
	if (ast->cmd_arg[1] && ast->cmd_arg[2])
		return (ft_putstr_fd("minishell: cd: too many arguments\n", 2), 1);
	path = path_handle(ast, shell);
	if (!path)
		return (1);
	curr_dir = getcwd(NULL, 0);
	if (!curr_dir)
	{
		ft_putstr_fd("minishell: cd: error retrieving current directory\n", 2);
		return (1);
	}
	if (chdir(path) != 0)
	{
		printf("minishell: cd: %s: %s\n", path, strerror(errno));
		return (free(curr_dir), 1);
	}
	update_env_vars(&shell);
	return (free(curr_dir), 0);
}
