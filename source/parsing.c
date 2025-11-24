/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbiskin <rbiskin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/21 12:37:53 by rbiskin           #+#    #+#             */
/*   Updated: 2025/10/21 12:50:50 by rbiskin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static long	ato_long_pos(const char *s)
{
	long	n;
	int		i;

	n = 0;
	if (!s || !*s)
		return (-1);
	i = 0;
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (-1);
		n = n * 10 + (s[i] - '0');
		if (n > 2147483647)
			return (-1);
		i++;
	}
	return (n);
}

int	parse_args(int ac, char **av, t_rules *r)
{
	if (ac != 5 && ac != 6)
		return (0);
	r->n_philo = (int)ato_long_pos(av[1]);
	r->t_die = ato_long_pos(av[2]);
	r->t_eat = ato_long_pos(av[3]);
	r->t_sleep = ato_long_pos(av[4]);
	r->must_eat = -1;
	if (ac == 6)
		r->must_eat = (int)ato_long_pos(av[5]);
	if (r->n_philo <= 0 || r->t_die <= 0 || r->t_eat <= 0 || r->t_sleep <= 0
		|| (ac == 6 && r->must_eat <= 0))
		return (0);
	return (1);
}
