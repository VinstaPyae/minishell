#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

int main() {
    char *input;

    while (1) {
        // Display a prompt and read input
        input = readline("minishell> ");

        // Exit on EOF (Ctrl+D)
        if (!input) break;

        // Add input to history (optional)
        add_history(input);

        // Process the input (e.g., execute a command)
        printf("You entered: %s\n", input);

        // Free the input string (readline allocates memory dynamically)
        free(input);
    }

    return 0;
}