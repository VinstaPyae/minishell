/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 16:48:24 by pzaw              #+#    #+#             */
/*   Updated: 2024/06/04 16:20:04 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	char	*r;
	int		i;

	r = (char *) s;
	i = ft_strlen(r);
	while (i >= 0)
	{
		if (s[i] == (char)c)
			return ((r + i));
		i--;
	}
	return (NULL);
}

/* int	main(void)
{
	printf("%s \n", strrchr("fdasffdggr",'a'));
	printf("%s \n", ft_strrchr("fdasffdggr",'a'));
} */