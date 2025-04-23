#include "minishell.h"

void free_array_list(char **path_dirs, int index)
{
    if (!path_dirs)
        return;

    if (index < 0) {
        index = 0;
        while (path_dirs[index])  // Find the length first
            index++;

        while (index-- > 0)  // Free elements in reverse order
            free(path_dirs[index]);
    }
    else {
        while (index-- > 0)
            free(path_dirs[index]);
    }

    free(path_dirs);
}
void init_tmp(int *tmp)
{
	tmp[0] = 0;
	tmp[1] = 0;
	tmp[2] = 0;
	tmp[3] = 0;
	tmp[4] = 0;
}

int n_option_checked(const char *str)
{
	int i;

	if (str[0] != '-')
		return (0);
	i = 1;
	while (str[i] == 'n')
		i++;
	return (i > 1) && (str[i + 1] == '\0');
}