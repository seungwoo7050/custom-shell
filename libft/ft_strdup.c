#include "libft.h"

char	*ft_strdup(char *s)
{
	char	*new_s;
	int		i;

	i = 0;
	if (s == 0)
		return (0);
	new_s = (char *)malloc(sizeof(char) * (ft_strlen(s) + 1));
	if (new_s == 0)
		return (0);
	while (s[i])
	{
		new_s[i] = s[i];
		i++;
	}
	new_s[i] = '\0';
	return (new_s);
}
