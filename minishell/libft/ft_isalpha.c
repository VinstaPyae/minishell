/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalpha.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jace <jace@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/19 18:10:30 by pzaw              #+#    #+#             */
/*   Updated: 2024/06/03 23:01:22 by jace             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isalpha(int c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return (1024);
	return (0);
}

/* int    main(void)
 {
	int c;
	c = 'C';
	int r = isalpha(c);
	printf("%d \n", r);
	int z = ft_isalpha(c);
	printf("%d \n", z);
 } */