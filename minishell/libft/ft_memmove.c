/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 14:21:54 by pzaw              #+#    #+#             */
/*   Updated: 2024/06/11 15:11:12 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	size_t			i;
	unsigned char	*d;
	unsigned char	*s;

	i = -1;
	d = (unsigned char *)dest;
	s = (unsigned char *)src;
	if (d == NULL && s == NULL)
		return (dest);
	if (d < s)
		while (++i < n)
			d[i] = s[i];
	else
		while (n-- > 0)
			d[n] = s[n];
	return (dest);
}

/* int main(void)
{
    char str1[50] = "Hello, world!";
    char str2[50] = "Goodbye, world!";
    printf("Before ft_memmove:\n");
    printf("str1: %s\n", str1);
    printf("str2: %s\n", str2);
    memmove(str2, str1, 13);
    printf("\nAfter ft_memmove:\n");
    printf("str1: %s\n", str1);
    printf("str2: %s\n", str2);
} */