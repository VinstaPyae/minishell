#include "minishell.h"

void print_error(const char *func_name, const char *file, int line, const char *format, ...)
{
    va_list args;

    // Print the error header
    fprintf(stderr, "Error in %s (%s:%d): ", func_name, file, line);

    // Print the custom error message
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    // Add a newline for readability
    fprintf(stderr, "\n");
}

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
