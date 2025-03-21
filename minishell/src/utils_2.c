#include "minishell.h"

char **trim_cmd(char **cmd_arg)
{
	char **cmd;
	int i;

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
volatile sig_atomic_t g_signal_status = 0;

void handle_sigint(int signo)
{
	(void)signo;
	if (g_signal_status == 2) // Child is running, mark the signal
	{
		g_signal_status = 1;
		// No newline here - just mark the signal
		return;
	}

	// Refresh the prompt when shell is waiting for input
	rl_replace_line("", 0);
	rl_crlf();
	rl_on_new_line();
	rl_redisplay();
}

void handle_sigquit(int signo)
{
	(void)signo;
	// Do nothing for SIGQUIT at the shell level.
}

void setup_signal_handlers(void)
{
	struct sigaction sa;

	/* Handle SIGINT (Ctrl-C) */
	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART; // Restart interrupted syscalls like readline
	sigaction(SIGINT, &sa, NULL);

	/* Ignore SIGQUIT (Ctrl-\) */
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);

	/* Ignore SIGTSTP (Ctrl-Z) so the shell is not stopped */
	sigaction(SIGTSTP, &sa, NULL);
}

