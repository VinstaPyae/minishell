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