/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tolower.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jace <jace@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 17:37:12 by pzaw              #+#    #+#             */
/*   Updated: 2024/05/25 02:43:08 by jace             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_tolower(int c)
{
	if (c >= 65 && c <= 90)
	{
		c += 32;
	}
	return (c);
}

/* int	main(void)
{
	printf("%d \n", tolower('Z'));
	printf("%d \n", tolower('a'));
	printf("%d \n", ft_tolower('Z'));
	printf("%d \n", ft_tolower('a'));
} */