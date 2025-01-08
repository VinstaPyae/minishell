/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 16:46:52 by pzaw              #+#    #+#             */
/*   Updated: 2024/06/07 22:17:15 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t			i;
	unsigned char	*str1;
	unsigned char	*str2;

	i = 0;
	str1 = (unsigned char *)s1;
	str2 = (unsigned char *)s2;
	if (!n)
		return (0);
	while (str1[i] == str2[i] && i < (n - 1) && str1[i])
		i++;
	return (str1[i] - str2[i]);
}

/* int	main(void)
{
	printf("%d \n", strncmp("fdsdfaaggr","fdzsfaaggr",3));
        printf("%d \n", ft_strncmp("fdsdfaaggr","fdzsfaaggr",3));
} */