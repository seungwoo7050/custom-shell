#include "libft.h"

t_list	*ft_lstnew(void *content)
{
	t_list	*newlst;

	newlst = (t_list *)malloc(sizeof (t_list));
	if (newlst == 0)
		return (0);
	newlst -> content = content;
	newlst -> next = 0;
	return (newlst);
}
