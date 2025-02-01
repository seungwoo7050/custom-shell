#include "libft.h"

void	*ft_calloc(int nmemb, int size)
{
	void	*p;

	p = malloc(nmemb * size);
	if (p == 0)
		return (0);
	ft_bzero(p, nmemb * size);
	return (p);
}
