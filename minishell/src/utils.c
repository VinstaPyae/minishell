#include "minishell.h"

int	ft_isspace(int c)
{
	if (c == ' ' || c == '\n' || c == '\t' || c == '\v' || c == '\f' ||
		c == '\r')
		return (1);
	return (0);
}

int	variable_len(char *input)
{
	int	i;

	if (!input || input[0] != '$') // Check if input is valid and starts with '$'
        	return (0);
	i = 1;
	if (input[i] >= '0' && input[i] <= '9')
		return (2);
	while (input[i] && (ft_isalnum(input[i]) || ft_isalpha(input[i]) || input[i] == '_'))
	{
		if (ft_isspace(input[i]) || (input[i] == '\'' || input[i] == '"' || input[i] == '$'))
			break;
		i++;
	}
	return (i);
}

int	ot_len(char *input)
{
	if (*input == '\'' || *input == '"' || *input == '|' || *input == '$')
		return (1);
	if ((*input == '>' && *(input + 1) != '>') || (*input == '<' && *(input + 1) != '<'))
		return (1);
	if ((*input == '>' && *(input + 1) == '>') || (*input == '<' && *(input + 1) == '<'))
		return (2);
	return (0);
}

int	wd_len(char *input)
{
	int	i;

	i = 0;
	while (input[i])
	{
		if (input[i] == 32 || (input[i] >= 9 && input[i] <= 13))
			break;
		if (ot_len(&input[i]) > 0)
			break;
		i++;
	}
	return (i);
}

// int	quote_len(char *str)
// {
// 	int		i;
// 	char	quote_char;

// 	i = 0;
// 	if (str[i] == '\'' || str[i] == '"')
// 	{
// 		quote_char = str[i]; // Store the opening quote type
// 		i++;
// 		while (str[i] && str[i] != quote_char) // Look for the matching closing quote
// 			i++;
// 		if (str[i] == quote_char) // Ensure a closing quote was found
// 			return (i + 1);
// 		else
// 			return (0); // Return 0 if no matching closing quote is found
// 	}
// 	return (0);
// }

int	quote_len(char *str)
{
	int i = 0;
	char quote_char;

	if (str[i] == '\'' || str[i] == '"')
	{
		quote_char = str[i];
		i++;
		while (str[i] && str[i] != quote_char)
			i++;
		if (str[i] == quote_char)
			return (i + 1); // Include the closing quote
		else
			return -1; // Unclosed quote
	}
	return 0;
}

/*int quote_len(char *str)
{
	int i;
	char quote_char;

	i = 0;
	if (str[i] == '\'' || str[i] == '"')
	{
		quote_char = str[i];
		i++;
		while (str[i])
		{
			if (str[i] == '\\' && (str[i + 1] == quote_char || str[i + 1] == '\\'))
			{
				i += 2; // Skip the escape character and the escaped quote or backslash
				continue;
			}
			if (str[i] == quote_char)
			{
				return (i + 1); // Return length including the closing quote
			}
		i++;
		}
		return (-1); // Unclosed quote
	}
	return (0); // Not a quote
}*/

int	ft_strcmp(const char *s1, const char *s2)
{
	if (!s1)
		return (-1);
	if (!s2)
		return (1);
	while (*s1 && *s2 && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return ((unsigned char)*s1 - (unsigned char)*s2);
}
