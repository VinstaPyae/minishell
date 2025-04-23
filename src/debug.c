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
