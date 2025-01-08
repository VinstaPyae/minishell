/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 18:01:51 by pzaw              #+#    #+#             */
/*   Updated: 2024/06/07 21:42:28 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	l_len;
	char	*b;
	char	*l;

	b = (char *)big;
	l = (char *)little;
	if (!*l)
		return ((char *)b);
	l_len = ft_strlen(l);
	while (*b && len >= l_len)
	{
		if (ft_strncmp(b, l, l_len) == 0)
			return ((char *) b);
		b++;
		len--;
	}
	return (NULL);
}

/* int main(void)
{
	const char *largestring = "Foo Bar Baz";
	const char *smallstring = "Baa";

	char *pp = ft_strnstr(largestring, smallstring, sizeof(largestring));
	printf("%s\n", pp);
} */