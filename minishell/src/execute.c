#include "minishell.h"

//builtin check and execute
int	builtin_cmd_check(t_ast_node **node)
{
	int	r;

	r = 0;
	if (!node || !(*node)->cmd)
	{
		printf("builin check here???");
			return (1);
	}
	if (ft_strncmp((*node)->cmd, "echo", 5) == 0)
		r = exe_echo(node);
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
int exe_cmd(t_ast_node **node)
{
    if (!(*node))
    {
        printf("Error: No command node provided\n");
        return 1;
    }
    // printf("Executing command: %s\n", (*node)->cmd);
    return builtin_cmd_check(node);
}

////all execution 
int	execute_ast(t_ast_node **node)
{
	int	r;

	r = 0;
	if (!(*node))
		return (1);
	if ((*node)->type == NODE_COMMAND)
	{
		// printf("Executing command TYPE: %d\n", (*node)->type);
		// printf("Executing command CMD: %s\n", (*node)->cmd);
		r = exe_cmd(node);
	}
	return (r);
}