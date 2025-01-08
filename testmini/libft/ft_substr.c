/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 14:22:34 by pzaw              #+#    #+#             */
/*   Updated: 2024/06/10 19:32:35 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*ptr;
	size_t	i;
	size_t	len_s;
	size_t	og_len;

	len_s = ft_strlen(s);
	og_len = len;
	if (!s)
		return (NULL);
	if (start > len_s)
		return (ft_strdup(""));
	else if (start + len > len_s)
		og_len = len_s - start;
	ptr = (char *)malloc((og_len + 1) * sizeof(char));
	if (!ptr)
		return (NULL);
	i = -1;
	while (++i < len && s[start + i] != '\0')
		ptr[i] = s[start + i];
	ptr[i] = '\0';
	return (ptr);
}

/* int	main(void)
{
	char s[] = "abcdefgh";
	char *r = ft_substr(s, 6, 6);
	printf("%s \n", r);
}
 */