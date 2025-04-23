#include "minishell.h"

char *extract_heredoc_name(const char *input, int *i)
{
	char *var_name;
	int start;
	int j;

	start = *i;
	j = start + 1;
	if (input[j] == '?')
		j++;
	else if (ft_isdigit(input[j]))
		j++;
	else
	{
		if (ft_isalpha(input[j]) || input[j] == '_')
		{
			j++;
			while (ft_isalnum(input[j]) || input[j] == '_')
				j++;
		}
		else
			j = start + 1;
	}
	var_name = ft_substr(input, start, j - start);
	*i = j;
	return (var_name);
}

char *expand_heredoc(char *input, t_minishell *shell)
{
	char *result;
	char *var_name;
	int i;

	i = 0;
	result = ft_strdup("");
	while (input[i])
	{
		if (input[i] == '$')
		{
			var_name = extract_heredoc_name(input, &i);
			if (ft_strlen(var_name) > 1)
				result = append_expanded_heredoc(result, var_name, shell);
			else
				result = append_normal_character(result, '$');
		}
		else
		{
			result = append_normal_character(result, input[i]);
			i++;
		}
	}
	return (result);
}

// Modified handle_heredoc function
// ...existing code...
// Function to handle the expansion of a heredoc line
char *process_heredoc_line(char *line, char *delimiter, t_minishell *shell, int *should_break)
{
	char *expanded_line;

	expanded_line = expand_heredoc(line, shell);
	if (g_signal_status == 130) // Signal interrupt
	{
		free(line);
		free(expanded_line);
		*should_break = 1;
		return NULL;
	}
	if (ft_strcmp(line, delimiter) == 0) // End of heredoc
	{
		free(line);
		free(expanded_line);
		*should_break = 1;
		return NULL;
	}
	free(line);
	return expanded_line;
}

// Function to write the expanded line to the pipe
void write_expanded_line_to_pipe(char *expanded_line, int pipefd)
{
	if (expanded_line)
	{
		write(pipefd, expanded_line, ft_strlen(expanded_line));
		write(pipefd, "\n", 1);
		free(expanded_line);
	}
}

void process_eof_warning(char *delimiter)
{
	write(STDERR_FILENO, "minishell: warning: here-document delimited by end-of-file (wanted `", 68);
	write(STDERR_FILENO, delimiter, ft_strlen(delimiter));
	write(STDERR_FILENO, "')\n", 3);
}
