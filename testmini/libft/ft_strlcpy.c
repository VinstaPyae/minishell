/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 19:07:32 by pzaw              #+#    #+#             */
/*   Updated: 2024/06/07 22:14:18 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *dest, const char *src, size_t size)
{
	size_t	i;
	size_t	src_len;

	i = 0;
	src_len = ft_strlen(src);
	if (size > 0)
	{
		while (src[i] && i < (size - 1))
		{
			dest[i] = src[i];
			i++;
		}
		dest[i] = '\0';
	}
	return (src_len);
}

/* int main(void)
{
        const char *s;
        char d[0xF00];
        size_t i;
        char *src = "BBBB";
        char des[0xF00];
        size_t j = 0;

        strlcpy(des, src, j);
        printf("%s \n", des);

        s = "BBBB";
        i = 0;
        ft_strlcpy(d, s, i);
        printf("%s \n", d);
} */