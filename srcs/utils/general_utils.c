#include "minishell.h"

char	*modified_strjoin_free(char *s1, char *s2, int flag)
{
	char	*str;
	int		i;
	int		k;
	int		len1;
	int		len2;

	len1 = s1 ? ft_strlen(s1) : 0;
	len2 = s2 ? ft_strlen(s2) : 0;
	str = ft_malloc(sizeof (char) * (len1 + len2 + 1));
	i = 0;
	if (s1)
	{
		while (s1[i])
		{
			str[i] = s1[i];
			i++;
		}
	}
	k = 0;
	if (s2)
	{
		while (s2[k])
		{
			str[i + k] = s2[k];
			k++;
		}
	}
	str[i + k] = 0;
	/* free according to flag (free(NULL) is safe) */
	if (flag == 1)
		free(s1);
	else if (flag == 2)
		free(s2);
	else if (flag == 0)
	{
		free(s1);
		free(s2);
	}
	return (str);
}

char	*join_path(char *base, char *suffix, int ownership)
{
	char	*path;
	char	*suffix_start;
	int		base_len;
	int		suffix_len;
	int		need_slash;

	if (!base && !suffix)
		return (NULL);
	suffix_start = suffix;
	base_len = 0;
	suffix_len = 0;
	if (base)
		base_len = ft_strlen(base);
	if (suffix)
		suffix_len = ft_strlen(suffix);
	need_slash = 0;
	if (base_len && suffix_len)
	{
		if (base[base_len - 1] != '/' && suffix[0] != '/')
			need_slash = 1;
		else if (base[base_len - 1] == '/' && suffix[0] == '/')
		{
			suffix++;
			suffix_len--;
		}
	}
	path = ft_malloc(sizeof(char) * (base_len + suffix_len + need_slash + 1));
	if (base_len)
		ft_strlcpy(path, base, base_len + 1);
	else
		path[0] = '\0';
	if (need_slash)
		path[base_len++] = '/';
	if (suffix_len)
		ft_strlcpy(path + base_len, suffix, suffix_len + 1);
	else
		path[base_len] = '\0';
	if (ownership == 0)
	{
		free(base);
		free(suffix_start);
	}
	else if (ownership == 1)
		free(base);
	else if (ownership == 2)
		free(suffix_start);
	return (path);
}

char	*modified_substr_free(char *s, int start, int len)
{
	char	*str;
	int		i;

	if (s == 0)
		return (0);
	if (start >= ft_strlen(s))
		return (ft_strdup(""));
	if (len > ft_strlen(s) - start)
		len = ft_strlen(s) - start;
	str = (char *)ft_malloc(sizeof (char) * (len + 1));
	i = 0;
	while (i < len && s[start + i])
	{
		str[i] = s[start + i];
		i++;
	}
	str[i] = 0;
	free(s);
	return (str);
}

char	*modified_strtrim_free(char *s1, char const *set)
{
	int		start;
	int		end;
	char	*str;

	if (set == 0)
		return ((char *)s1);
	start = -1;
	end = ft_strlen(s1) + 1;
	while (s1 && s1[++start])
		if (!ft_strchr(set, s1[start]))
			break ;
	while (--end > 0)
		if (!ft_strchr(set, s1[end - 1]))
			break ;
	if (start > end)
	{
		free(s1);
		return (NULL);
	}
	str = ft_malloc(end - start + 1);
	ft_strlcpy(str, s1 + start, end - start + 1);
	free(s1);
	return (str);
}

char	*modified_strdup_free(char *s)
{
	char	*new_s;
	int		i;

	i = 0;
	if (s == 0)
		return (0);
	new_s = (char *)ft_malloc(sizeof(char) * (ft_strlen(s) + 1));
	while (s[i])
	{
		new_s[i] = s[i];
		i++;
	}
	new_s[i] = s[i];
	free(s);
	return (new_s);
}

void	*ft_malloc(int size)
{
	void	*ptr;

	ptr = malloc(size);
	if (ptr == NULL)
	{
		perror("minishell");
		exit(E_MALLOC);
	}
	return (ptr);
}
