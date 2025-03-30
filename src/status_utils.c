#include "minishell.h"

void set_exit_status(t_minishell *shell, int status)
{
    if (shell)
        shell->exit_status = status;
}

int return_with_status(t_minishell **shell, int status)
{
    if (shell && *shell)
        (*shell)->exit_status = status;
    return (status);
}

int return_error(t_minishell **shell, const char *msg, int status)
{
    if (msg)
        ft_putstr_fd((char *)msg, STDERR_FILENO);
    return (return_with_status(shell, status));
}
