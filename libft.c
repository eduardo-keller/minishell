#include "parser.h"

char	*ft_strdup(const char *s)
{
	char	*s_dup;
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	s_dup = (char *)ft_calloc(i + 1, sizeof(char));
	if (s_dup == NULL)
		return (NULL);
	i = 0;
	while (s[i])
	{
		s_dup[i] = s[i];
		i++;
	}
	s_dup[i] = '\0';
	return (s_dup);
}

void	*ft_calloc(size_t nmemb, size_t size)
{
	size_t	total_mem;
	void	*alloc_mem;
	size_t	max_size;

	max_size = ~(size_t)0;
	total_mem = nmemb * size;
	if (nmemb == 0 || size == 0)
		return (malloc(0));
	if (size != 0 && nmemb > max_size / size)
		return (NULL);
	alloc_mem = malloc(total_mem);
	if (alloc_mem == NULL)
		return (NULL);
	ft_memset(alloc_mem, 0, total_mem);
	return (alloc_mem);
}