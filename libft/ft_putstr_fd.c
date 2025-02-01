#include "libft.h"

void	ft_putstr_fd(const char *s, int fd)
{
	if (fd < 0 || s == 0)
		return ;
	write(fd, s, ft_strlen(s));
}
