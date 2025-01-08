/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_toupper.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 17:37:20 by pzaw              #+#    #+#             */
/*   Updated: 2024/06/10 21:46:15 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_toupper(int c)
{
	if (c >= 97 && c <= 122)
	{
		c -= 32;
	}
	return (c);
}

/* int	main(void)
{
	printf("%d \n", toupper('A'));
	printf("%d \n", toupper('9'));
	printf("%d \n", ft_toupper('A'));
	printf("%d \n", ft_toupper('9'));
} */