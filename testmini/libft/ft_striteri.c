/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_striteri.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 14:22:07 by pzaw              #+#    #+#             */
/*   Updated: 2024/06/11 15:24:28 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_striteri(char *s, void (*f)(unsigned int, char *))
{
	unsigned int	i;

	i = 0;
	while (i < ft_strlen(s))
	{
		(*f)(i, &s[i]);
		i++;
	}
}

/* void my_func(unsigned int i, char *str)
{
        printf("Call function: i = %d and s = %c \n", i, *str);
	return str + 32;
}

int main()
{
        char str[10] = "hello";
        printf("Before Function %s\n", str);
        ft_striteri(str, my_func);
        printf("After Function %s\n", str);
        return 0;
} */