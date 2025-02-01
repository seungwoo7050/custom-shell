#include "minishell.h"

void	signal_ignore(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

void	sig_handler(int signum)
{
	(void)signum;
	if (signum == SIGINT)
	{
		printf("\n");
		rl_replace_line("", 0);
		rl_on_new_line();
		rl_redisplay();
		g_received_signal = signum;
	}
	else if (signum == SIGQUIT)
		g_received_signal = signum;
}

void	sig_handler_heredoc(int signum)
{
	if (signum == SIGINT)
	{
		printf("\n");
		rl_replace_line("", 0);
		rl_on_new_line();
		g_received_signal = signum;
	}
}

void	sig_handler_waiting(int signum)
{
	(void)signum;
	if (g_received_signal == SIGINT)
		exit (EXIT_STATUS_SIGINT);
	else if (g_received_signal == SIGQUIT)
		exit (EXIT_STATUS_SIGQUIT);
}

void	sig_handler_exec(int signum)
{
	if (signum == SIGQUIT)
	{
		rl_on_new_line();
		printf("Quit: 3\n");
		rl_replace_line("", 0);
		g_received_signal = signum;
	}
	else if (signum == SIGINT)
	{
		printf("\n");
		rl_replace_line("", 0);
		rl_on_new_line();
		g_received_signal = signum;
	}
}
