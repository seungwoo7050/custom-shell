#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*result_list;
	t_list	*new;
	void	*content;

	result_list = 0;
	while (lst != 0)
	{
		content = f(lst -> content);
		new = ft_lstnew(content);
		if (new == 0)
		{
			del(content);
			ft_lstclear(&result_list, del);
			return (0);
		}
		ft_lstadd_back(&result_list, new);
		lst = lst -> next;
	}
	return (result_list);
}
