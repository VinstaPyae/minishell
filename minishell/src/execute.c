#include "minishell.h"

//builtin check and execute
int	builtin_cmd_check(t_minishell **shell)
{
	int	r;

	r = 0;
	//printf("execute_ast: executing command: %s\n", (*shell)->ast->cmd); // Debug print
	if (!shell || !*shell || !(*shell)->ast)
        return (1);
	if (ft_strncmp((*shell)->ast->cmd, "echo", 5) == 0)
		r = exe_echo(shell);
	else if (ft_strncmp((*shell)->ast->cmd, "env", 4) == 0)
		r = exe_env(shell);
	else if (ft_strncmp((*shell)->ast->cmd, "unset", 6) == 0)
    	r = exe_unset(shell);
	else if (ft_strncmp((*shell)->ast->cmd, "exit", 5) == 0)
		r = exe_exit(shell);
	// else if (ft_strcmp(node->cmd_arg[0], "cd") == 0)
    //     	r = exe_cd(node);
	// else if (ft_strcmp(node->cmd_arg[0], "pwd") == 0)
	// 	r = exe_pwd();
	// else if (ft_strcmp(node->cmd_arg[0], "export") == 0)
	// 	r = exe_export(node);
	// else if (ft_strcmp(node->cmd_arg[0], "unset") == 0)
	// 	r = exe_unset(node);
	// else if (ft_strcmp(node->cmd_arg[0], "env") == 0)
	// 	r = exe_env();
	// else if (ft_strcmp(node->cmd_arg[0], "exit") == 0)
	// 	r = exe_exit(node);
	else
		r = -1;
	return (r);
}

//all command check and execute
int exe_cmd(t_minishell **shell)
{
    if (!(*shell)->ast)
    {
        printf("Error: No command node provided\n");
        return 1;
    }
    // printf("Executing command: %s\n", (*node)->cmd);
    return builtin_cmd_check(shell);
}

////all execution 
int	execute_ast(t_minishell **shell)
{
	int	r;

	r = 0;
	if (!(*shell)->ast)
		return (1);
	if ((*shell)->ast->type == NODE_COMMAND)
	{
		// printf("Executing command TYPE: %d\n", (*node)->type);
		// printf("Executing command CMD: %s\n", (*node)->cmd);
		r = exe_cmd(shell);
	}
	return (r);
}
