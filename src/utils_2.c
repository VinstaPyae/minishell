#include "minishell.h"

char **trim_cmd(char **cmd_arg)
{
	char **cmd;
	int i;

	i = 0;
	if (!cmd_arg || !cmd_arg[0])
		return (NULL);
	while (cmd_arg[i])
		i++;
	cmd = malloc((i + 1) * sizeof(char *));
	if (!cmd)
		return (NULL);
	i = 0;
	while (cmd_arg[i])
	{
		cmd[i] = ft_strtrim(cmd_arg[i], " ");
		if (!cmd[i])
			free_arg(cmd);
		i++;
	}
	cmd[i] = NULL;
	return (cmd);
}

char *trim_last_char(const char *s, char c)
{
	size_t len;
	char *trimmed;

	if (!s)
		return NULL;

	len = ft_strlen(s);

	// If the string is not empty and the last character matches 'c'
	if (len > 0 && s[len - 1] == c)
		len--; // Reduce the length by one

	trimmed = (char *)malloc(len + 1);
	if (!trimmed)
		return NULL;

	// Copy the appropriate number of characters
	ft_memcpy(trimmed, s, len);
	trimmed[len] = '\0';

	return trimmed;
}

volatile sig_atomic_t g_signal_status = 0;

void handle_sigint(int signo)
{
	(void)signo;
	if (g_signal_status == 2)
	{ // Child is running
		//g_signal_status = 1;
		return;
	}
	// Only print newline and redisplay if at prompt
	//write(STDOUT_FILENO, "Holy Relic", 12);
	write(STDOUT_FILENO, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
	g_signal_status = 130;
}



// Add these two helper functions
int check_sigint(void)
{
	if (g_signal_status == 130)
	{
		rl_done = 1;
		return (1);
	}
	return (0);
}

void handle_sigint_heredoc(int signo)
{
	(void)signo;
	g_signal_status = 130;
	rl_replace_line("", 0);
	rl_done = 1;
}

// void handle_sigquit(int signo)
// {
//     (void)signo;
//     if (g_signal_status == 2)
//     { // Child is running
//         // Do nothing, let the default action occur in the child
//         return;
//     }
    
//     // For the parent shell, completely ignore SIGQUIT
//     // But also handle the terminal display issue
//     rl_on_new_line();
//     rl_redisplay();
// }

void setup_signal_handlers(void)
{
	struct sigaction sa;

	signal(SIGQUIT, SIG_IGN);
	/* Handle SIGINT (Ctrl-C) */
	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART; // Restart interrupted syscalls like readline
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTSTP, &sa, NULL);
}
void handle_eof(t_minishell *shell)
{
	int saved_exit_status;

	if(g_signal_status == 130)
		saved_exit_status = 130;
	else
		saved_exit_status = shell->exit_status;
	if (isatty(STDIN_FILENO)) // Only print if we're in interactive mode
		printf("exit\n");

	// Cleanup before exiting
	cleanup(&shell);
	if (shell->envp)
		free_env_list(shell->envp);
	if (shell->input)
		free(shell->input);
	rl_clear_history();
	free(shell);
	exit(saved_exit_status);
}
