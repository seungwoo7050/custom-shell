#include "libft.h"

void	ft_bzero(void *s, int n)
{
	int		i;
	char	*p;

	i = 0;
	p = (char *)s;
	while (i < n)
	{
		p[i] = 0;
		i++;
	}
}
