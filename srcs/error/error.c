#include "minishell.h"

static void put_err(const char* msg) {
    ft_putstr_fd(msg, 2);
}

void	error_quote(t_arg *arg)
{
	put_err ("minishell: syntax error either unclosed quote ");
	put_err ("or parenthesis found\n");
	arg->error->code = EXIT_STATUS_SYNTAX_ERROR;
	if (arg->is_subshell)
		exit(arg->error->code);
}

void	error_syntax(t_arg *arg)
{
	if (arg->is_subshell)
		exit(arg->error->code);
	if (arg->error->token[0] == '$')
	{
		put_err("minishell: ");
		put_err(arg->error->token);
		put_err(": ambiguous redirect\n");
	}
	else if (arg->error->token[0] == '\'')
		put_err("minishell: No such file or directory\n");
	else if (arg->error->token[0] == '\0')
		put_err("minishell: ambiguous redirect\n");
	else
	{
		put_err("minishell: syntax error near unexpected token `");
		put_err(arg->error->token);
		put_err("'\n");
	}
	arg->error->code = EXIT_STATUS_SYNTAX_ERROR;
}

void	error_handler(t_arg *arg, int type)
{
	if (type == E_WRONG_SYNTAX)
		error_syntax(arg);
	else if (type == E_UNCLOSED_QUOTES)
		error_quote(arg);
}
