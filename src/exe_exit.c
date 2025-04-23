#include "minishell.h"

static int ft_atoll(const char *str, long long *num)
{
    int index;
    int sign;
    long long res;

    if (!skip_whitespace_and_sign(str, &sign, &index))
        return 0;
    if (handle_llong_min(str, sign, index, num))
        return 1;
    if (!convert_to_long_long(str, index, &res))
        return 0;
    *num = res * sign;
    return 1;
}

static void handle_exit_cleanup(t_minishell **shell, int exit_status)
{
    reset_close_fd((*shell)->og_fd); // Reset file descriptors
    cleanup(shell); // Final cleanup
    if ((*shell)->envp)
        free_env_list((*shell)->envp);
    if (*shell)
        free(*shell);
    rl_clear_history();
    exit(exit_status);
}

static void handle_numeric_error(t_minishell **shell,char *arg)
{
    ft_putstr_fd("exit: ", STDERR_FILENO);
    ft_putstr_fd(arg, STDERR_FILENO);
    ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
    handle_exit_cleanup(shell, 2);
}

static int count_arguments(char **args)
{
    int count;

    count = 0;
    while (args[count])
        count++;
    return (count);
}

int exe_exit(t_minishell **shell, t_ast_node *ast)
{
    char **args;
    int arg_count;
    int saved_exit_status;
    long long exit_num;

    args = ast->cmd_arg;
    arg_count = count_arguments(args);
    if (g_signal_status == 130)
        (*shell)->exit_status = 130; // Reset after handling
    saved_exit_status = (*shell)->exit_status;
    printf("exit\n");
    if (arg_count == 1)
        handle_exit_cleanup(shell, saved_exit_status);
    if (!ft_atoll(args[1], &exit_num))
        handle_numeric_error(shell, args[1]);
    if (arg_count > 2)
    {
		reset_close_fd((*shell)->og_fd); // Reset file descriptors
		ft_putstr_fd("exit: too many arguments\n", STDERR_FILENO);
        return (1); // Don't exit shell
    }
    handle_exit_cleanup(shell, (unsigned char)exit_num);
    return (0); // This line will never be reached
}
