#include "libft.h"

void	*ft_memcpy(void *dest, const void *src, int n)
{
	int				i;
	unsigned char	*destp;
	unsigned char	*srcp;

	i = 0;
	destp = (unsigned char *)dest;
	srcp = (unsigned char *)src;
	if (dest == 0)
		return (0);
	if (dest == src || src == 0)
		return (dest);
	while (i < n)
	{
		destp[i] = srcp[i];
		i++;
	}
	return (dest);
}
