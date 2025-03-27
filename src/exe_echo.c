#include "minishell.h"

int n_option_checked(const char *str)
{
	int i;

	if (str[0] != '-')
		return (0);
	i = 1;
	while (str[i] == 'n')
		i++;
	return (i > 1);
}

// echo builtin
int exe_echo(t_minishell **shell)
{
	int i;
	int newline;

	i = 1;
	newline = 1;
	// Check if node or cmd_arg is NULL
	if (!(*shell)->ast || !(*shell)->ast->cmd_arg[0])
		return 1;

	// If cmd_arg exists and has the -n option
	if ((*shell)->ast->cmd_arg && (*shell)->ast->cmd_arg[1] && n_option_checked((*shell)->ast->cmd_arg[1]))
	{
		newline = 0;
		i++;
	}
	// Print arguments if they exist
	if ((*shell)->ast->cmd_arg)
	{
		while ((*shell)->ast->cmd_arg[i])
		{
			// printf("Printing at  cmd_arg[%d]: Word: %s\n", i, (*shell)->ast->cmd_arg[i]);
			printf("%s", (*shell)->ast->cmd_arg[i]);
			i++;
		}
	}

	if (newline)
		printf("\n");
	// printf("Executing echo with arguments:\n");
	return (0);
}
/*
// Helper function to skip leading whitespace and handle sign
int parse_sign_and_whitespace(const char *str, int *sign, int *i)
{
	*sign = 1;
	*i = 0;

	// Skip leading whitespace
	while (ft_isspace(str[*i]))
		(*i)++;

	// Handle optional sign
	if (str[*i] == '-' || str[*i] == '+')
	{
		*sign = (str[*i] == '-') ? -1 : 1;
		(*i)++;
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

static int is_llong_min(const char *str, int start)
{
	const char *llmin = "9223372036854775808";
	int i = 0;

	while (llmin[i])
	{
		if (str[start + i] != llmin[i])
			return 0;
		i++;
	}
	return (str[start + i] == '\0' || ft_isspace(str[start + i]));
}

static int ft_atoll(const char *str, long long *num)
{
	int i = 0, sign = 1;

	while (ft_isspace(str[i]))
		i++;
	if (str[i] == '-' || str[i] == '+')
		sign = (str[i++] == '-') ? -1 : 1;
	if (!ft_isdigit(str[i]))
		return 0;

	// Handle LLONG_MIN explicitly
	if (sign == -1 && is_llong_min(str, i))
	{
		*num = LLONG_MIN;
		return 1;
	}

	long long res = 0;
	while (ft_isdigit(str[i]))
	{
		if (res > LLONG_MAX / 10 || (res == LLONG_MAX / 10 && (str[i] - '0') > 7))
			return 0;
		res = res * 10 + (str[i++] - '0');
	}

	// Check for trailing non-whitespace
	while (str[i])
	{
		if (!ft_isspace(str[i++]))
			return 0;
	}

	*num = res * sign;
	return 1;
}

int exe_exit(t_minishell **shell)
{
    char **args = (*shell)->ast->cmd_arg;
    int arg_count = 0;
    int saved_exit_status;
    long long exit_num;

    while (args[arg_count])
        arg_count++;

    // Save exit status before freeing *shell
    saved_exit_status = (*shell)->exit_status;

    // No arguments: exit with last status
    if (arg_count == 1)
    {
        cleanup(shell); // Final cleanup
        free_env_list((*shell)->envp);
        free(*shell);
        rl_clear_history();
        exit(saved_exit_status);
    }

    if (!ft_atoll(args[1], &exit_num))
    {
        cleanup(shell); // Final cleanup
        free_env_list((*shell)->envp);
        free(*shell);
        rl_clear_history();
        ft_putstr_fd("exit: ", STDERR_FILENO);
        ft_putstr_fd(args[1], STDERR_FILENO);
        ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
        exit(2);
    }

    if (arg_count > 2)
    {
        ft_putstr_fd("exit: too many arguments\n", STDERR_FILENO);
        (*shell)->exit_status = 1;
        printf("Exit Status: %d\n", (*shell)->exit_status);
        return (1); // Don't exit shell
    }

    cleanup(shell); // Final cleanup
    free_env_list((*shell)->envp);
    free(*shell);
    rl_clear_history();
    exit((unsigned char)exit_num);
}
