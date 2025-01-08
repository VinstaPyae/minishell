/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isprint.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jace <jace@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 16:44:10 by pzaw              #+#    #+#             */
/*   Updated: 2024/05/25 02:40:41 by jace             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isprint(int c)
{
	if (c >= 32 && c <= 126)
		return (16384);
	return (0);
}

/* int	main(void)
{
	printf("%d \n", isprint(32));
	printf("%d \n", isprint(127));
	printf("%d \n", ft_isprint(32));
	printf("%d \n", ft_isprint(127));
} */