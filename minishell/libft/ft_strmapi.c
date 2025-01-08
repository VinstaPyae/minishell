/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 14:22:20 by pzaw              #+#    #+#             */
/*   Updated: 2024/06/11 15:25:12 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	size_t		i;
	char		*res;

	res = (char *) malloc ((ft_strlen(s) + 1) * sizeof(char));
	if (!res)
		return (NULL);
	i = 0;
	while (i < ft_strlen(s))
	{
		res[i] = (*f)(i, s[i]);
		i++;
	}
	res[i] = '\0';
	return (res);
}

/* char my_func(unsigned int i, char str)
{
 	printf("Call function: i = %d and s = %c \n", i, str);
 	return str - 32;
}

int main()
{
 	char str[10] = "hello";
 	printf("Before Function %s\n", str);
 	char *result = ft_strmapi(str, my_func);
 	printf("After Function %s\n", result);
 	return 0;
} */
