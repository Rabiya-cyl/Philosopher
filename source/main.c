/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcaylan <rcaylan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 08:34:21 by rbiskin           #+#    #+#             */
/*   Updated: 2025/10/13 13:54:16 by rcaylan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

// si un seule philo
static int	si_un_seule_fils_de_pute(t_rules *r)
{
	print_state(r, 1, "has token a fork");
	usleep(r->t_die * 1000);
	print_state(r, 1, "died");
	return (0);
}

int	main(int ac, char **av)
{
	t_rules	r;
	t_philo	*ph;
	long	current_time;
	int		i;

	ph = NULL;
	if (!parse_args(ac, av, &r))
		return (printf("Error: bad gay\n"), 1);
	r.stop = 0;
	r.start_ms = timestamp_ms();
	if (!init_all(&r, &ph))
		return (printf("Error: init\n"), 1);
	if (r.n_philo == 1)
		return (si_un_seule_fils_de_pute(&r), destroy_all(&r, ph), 0);
	/* Mise à jour du temps juste avant de créer les threads */
	current_time = timestamp_ms();
	for (int j = 0; j < r.n_philo; j++)
		ph[j].last_meal_ms = current_time;
	/* Créer les threads des philosophes */
	i = 0;
	while (i < r.n_philo)
	{
		if (pthread_create(&ph[i].thread, NULL, routine, &ph[i]) != 0)
			return (printf("Error: thread\n"), destroy_all(&r, ph), 1);
		i++;
	}
	/* Créer le thread monitor */
	if (pthread_create(&r.monitor_thread, NULL, monitor_routine, &r) != 0)
	{
		pthread_mutex_lock(&r.stop_mx);
		r.stop = 1;
		pthread_mutex_unlock(&r.stop_mx);
		i = 0;
		while (i < r.n_philo)
			pthread_join(ph[i++].thread, NULL);
		return (destroy_all(&r, ph), 1);
	}
	/* Attendre que tous les threads se terminent */
	i = 0;
	while (i < r.n_philo)
		pthread_join(ph[i++].thread, NULL);
	pthread_join(r.monitor_thread, NULL);
	destroy_all(&r, ph);
	return (0);
}
