/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbiskin <rbiskin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/21 12:37:49 by rbiskin           #+#    #+#             */
/*   Updated: 2025/10/22 06:37:59 by rbiskin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	si_un_seule_fils_de_pute(t_rules *r)
{
	print_state(r, 1, "has token a fork");
	usleep(r->t_die * 1000);
	print_state(r, 1, "died");
	return (0);
}

static int	create_threads(t_rules *r, t_philo *ph)
{
	int	i;

	i = 0;
	while (i < r->n_philo)
	{
		if (pthread_create(&ph[i].thread, NULL, routine, &ph[i]) != 0)
			return (0);
		i++;
	}
	if (pthread_create(&r->monitor_thread, NULL, monitor_routine, r) != 0)
	{
		pthread_mutex_lock(&r->stop_mx);
		r->stop = 1;
		pthread_mutex_unlock(&r->stop_mx);
		i = 0;
		while (i < r->n_philo)
			pthread_join(ph[i++].thread, NULL);
		return (0);
	}
	return (1);
}

static void	join_threads(t_rules *r, t_philo *ph)
{
	int	i;

	i = 0;
	while (i < r->n_philo)
		pthread_join(ph[i++].thread, NULL);
	pthread_join(r->monitor_thread, NULL);
}

static void	init_last_meal_time(t_rules *r, t_philo *ph)
{
	long	current_time;
	int		i;

	current_time = timestamp_ms();
	i = 0;
	while (i < r->n_philo)
	{
		ph[i].last_meal_ms = current_time;
		i++;
	}
}

int	main(int ac, char **av)
{
	t_rules	r;
	t_philo	*ph;

	ph = NULL;
	if (!parse_args(ac, av, &r))
		return (printf("Error: bad gay\n"), 1);
	r.stop = 0;
	r.start_ms = timestamp_ms();
	if (!init_all(&r, &ph))
		return (printf("Error: init\n"), 1);
	if (r.n_philo == 1)
		return (si_un_seule_fils_de_pute(&r), destroy_all(&r, ph), 0);
	init_last_meal_time(&r, ph);
	if (!create_threads(&r, ph))
		return (printf("Error: thread\n"), destroy_all(&r, ph), 1);
	join_threads(&r, ph);
	destroy_all(&r, ph);
	return (0);
}
