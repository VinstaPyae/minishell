/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exe_echo.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 03:48:25 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 03:48:25 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	exe_echo(t_ast_node *ast)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	if (!ast || !ast->cmd_arg[0])
		return (1);
	while (ast->cmd_arg[i] && n_option_checked(ast->cmd_arg[i]))
	{
		newline = 0;
		i++;
	}
	if (ast->cmd_arg)
	{
		while (ast->cmd_arg[i])
		{
			printf("%s", ast->cmd_arg[i]);
			if (ast->cmd_arg[i + 1])
				printf(" ");
			i++;
		}
	}
	if (newline)
		printf("\n");
	return (0);
}

int	is_llong_min(const char *str, int start)
{
	const char	*llmin;
	int			i;

	llmin = "9223372036854775808";
	i = 0;
	while (llmin[i])
	{
		if (str[start + i] != llmin[i])
			return (0);
		i++;
	}
	return (str[start + i] == '\0' || ft_isspace(str[start + i]));
}

int	skip_whitespace_and_sign(const char *str, int *sign, int *index)
{
	*index = 0;
	*sign = 1;
	while (ft_isspace(str[*index]))
		(*index)++;
	if (str[*index] == '-' || str[*index] == '+')
	{
		if (str[*index] == '-')
			*sign = -1;
		(*index)++;
	}
	if (!ft_isdigit(str[*index]))
		return (0);
	return (1);
}

int	handle_llong_min(const char *str, int sign, int index, long long *num)
{
	if (sign == -1 && is_llong_min(str, index))
	{
		*num = LLONG_MIN;
		return (1);
	}
	return (0);
}

int	convert_to_long_long(const char *str, int index, long long *res)
{
	*res = 0;
	while (ft_isdigit(str[index]))
	{
		if (*res > LLONG_MAX / 10
			|| (*res == LLONG_MAX / 10 && (str[index] - '0') > 7))
			return (0);
		*res = *res * 10 + (str[index] - '0');
		index++;
	}
	while (str[index])
	{
		if (!ft_isspace(str[index]))
			return (0);
		index++;
	}
	return (1);
}

// int exe_exit(t_minishell **shell, t_ast_node *ast)
// {
// 	char **args = ast->cmd_arg;
// 	int arg_count = 0;
// 	int saved_exit_status;
// 	long long exit_num;

// 	while (args[arg_count])
// 		arg_count++;

// 	if(g_signal_status == 130)
// 		(*shell)->exit_status = 130; // Reset after handling
// 	saved_exit_status = (*shell)->exit_status;
// 	printf("exit\n");
// 	// No arguments: exit with last status
// 	if (arg_count == 1)
// 	{
// 		reset_close_fd((*shell)->og_fd); // Reset file descriptors
// 		cleanup(shell); // Final cleanup
// 		if ((*shell)->envp)
// 			free_env_list((*shell)->envp);
// 		if (*shell)
// 			free(*shell);
// 		rl_clear_history();
// 		exit(saved_exit_status);
// 	}

// 	if (!ft_atoll(args[1], &exit_num))
// 	{
// 		ft_putstr_fd("exit: ", STDERR_FILENO);
// 		ft_putstr_fd(args[1], STDERR_FILENO);
// 		ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
// 		reset_close_fd((*shell)->og_fd); // Reset file descriptors
// 		cleanup(shell); // Final cleanup
// 		if ((*shell)->envp)
// 			free_env_list((*shell)->envp);
// 		if (*shell)
// 			free(*shell);
// 		rl_clear_history();
// 		exit(2);
// 	}

// 	if (arg_count > 2)
// 	{
// 		reset_close_fd((*shell)->og_fd); // Reset file descriptors
// 		ft_putstr_fd("exit: too many arguments\n", STDERR_FILENO);
// 		return (1); // Don't exit shell
// 	}
// 	reset_close_fd((*shell)->og_fd); // Reset file descriptors
// 	cleanup(shell); // Final cleanup
// 	if ((*shell)->envp)
// 			free_env_list((*shell)->envp);
// 	if (*shell)
// 		free(*shell);
// 	rl_clear_history();
// 	exit((unsigned char)exit_num);
// }

/*
// Helper function to skip leading whitespace and handle sign
int parse_sign_and_whitespace(const char *str, int *sign, int *i)
{
	*sign = 1;
	*i = 0;
free_env;
	}

	// Check if string is empty or contains non-digit after sign
	if (str[*i] == '\0' || !ft_isdigit(str[*i]))
		return 0;

	return 1;
}

// Helper function to convert digits to long long with overflow check
int convert_digits_to_longlong(const char *str, int start, long long *result)
{
	long long value = 0;
	int i = start;

	while (ft_isdigit(str[i]))
	{
		// Check for potential overflow
		if (value > LLONG_MAX / 10 ||
			(value == LLONG_MAX / 10 && (str[i] - '0') > LLONG_MAX % 10))
		{
			// Overflow occurred
			return 0;
		}

		value = value * 10 + (str[i] - '0');
		i++;
	}

	// Check for trailing non-whitespace characters
	while (ft_isspace(str[i]))
		i++;
	if (str[i] != '\0')
		return 0;

	*result = value;
	return 1;
}

// Main function to convert string to long long
int ft_atoll(const char *str, long long *result)
{
	int sign, start;
	long long value;

	// Parse sign and initial whitespace
	if (!parse_sign_and_whitespace(str, &sign, &start))
		return 0;

	// Convert digits to long long
	if (!convert_digits_to_longlong(str, start, &value))
		return 0;

	// Apply sign and store result
	*result = sign * value;
	return 1;
}

// Combined validation and conversion function
int validate_and_convert_number(const char *str, long long *result)
{
	return ft_atoll(str, result);
}

// Normalize exit status to 0-255 range
unsigned char normalize_exit_status(long long status)
{
	// Wrap around for values outside 0-255 range
	return (unsigned char)(status % 256);
}

int exe_exit(t_minishell **shell)
{
	// No arguments: exit with last command's exit status
	if (!(*shell)->ast->cmd_arg[1])
	{
		cleanup(shell);
		rl_clear_history();
		exit((*shell)->exit_status);
	}

	// First argument
	char *first_arg = (*shell)->ast->cmd_arg[1];

	// Check for additional arguments
	int arg_count = 0;
	while ((*shell)->ast->cmd_arg[arg_count])
		arg_count++;

	long long exit_value;

	// Check if first argument is a valid number
	if (!validate_and_convert_number(first_arg, &exit_value))
	{
		// Non-numeric argument case
		fprintf(stderr, "exit: %s: numeric argument required\n", first_arg);
		cleanup(shell);
		rl_clear_history();
		exit(2);
	}

	// Too many arguments case
	if (arg_count > 2)
	{
		fprintf(stderr, "exit: too many arguments\n");
		return 1;
	}

	// Single valid numeric argument
	cleanup(shell);
	rl_clear_history();
	exit(normalize_exit_status(exit_value));
}*/