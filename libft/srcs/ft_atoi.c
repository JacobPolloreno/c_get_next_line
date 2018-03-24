/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpollore <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/24 08:58:09 by jpollore          #+#    #+#             */
/*   Updated: 2018/02/28 15:29:42 by jpollore         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int			ft_atoi(const char *str)
{
	long long	res;
	int			sign;

	sign = 1;
	res = 0;
	while (!(*str >= 48 && *str <= 57) && (*str != '-'))
	{
		if (((*str >= 9 && *str <= 13) || (*str == 32)) ||
				(*str == '+' && (str[1] >= 48 && str[1] <= 57)))
			str++;
		else
			return (0);
	}
	if (*str == '-' && str++)
		sign = -1;
	while (*str && (*str >= 48 && *str <= 57))
	{
		if (res >= 922337203685477580 && (*str - 48) > 7)
			return (sign == -1 ? 0 : -1);
		res = res * 10 + (*str - 48);
		str++;
	}
	return (res * sign);
}
