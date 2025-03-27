#include "minishell.h"

int exe_pwd(t_minishell **shell)
{
    char cwd[4096];

    (void)shell;

    if (getcwd(cwd, sizeof(cwd)))
    {
        printf("%s\n", cwd); 
        return (0);
    }
    else
    {
        perror("pwd");
        return (1);
    }
}
