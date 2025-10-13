/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcaylan <rcaylan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 08:47:42 by rbiskin           #+#    #+#             */
/*   Updated: 2025/10/13 13:49:25 by rcaylan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	init_mutexes(t_rules *r)
{
	int	i;

	if (pthread_mutex_init(&r->print_mx, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&r->death_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&r->bz_luca, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&r->monitor, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&r->stop_mx, NULL) != 0)
		return (0);
	r->forks = (pthread_mutex_t *)gg_malloc(r->gc, sizeof(pthread_mutex_t)
			* r->n_philo);
	if (!r->forks)
		return (0);
	i = 0;
	while (i < r->n_philo)
	{
		if (pthread_mutex_init(&r->forks[i], NULL) != 0)
			return (0);
		i++;
	}
	return (1);
}

int	init_all(t_rules *r, t_philo **ph)
{
	int	i;

	r->gc = gg_init();
	if (!r->gc)
		return (0);
	if (!init_mutexes(r))
		return (0);
	*ph = (t_philo *)gg_malloc(r->gc, sizeof(t_philo) * r->n_philo);
	if (!*ph)
		return (0);
	r->philosophers = *ph; /* Assigner le pointeur dans la structure rules */
	i = 0;
	while (i < r->n_philo)
	{
		(*ph)[i].id = i + 1;
		(*ph)[i].meals = 0;
		(*ph)[i].last_meal_ms = r->start_ms;
		(*ph)[i].rules = r;
		(*ph)[i].left_fork = &r->forks[i];
		(*ph)[i].right_fork = &r->forks[(i + 1) % r->n_philo];
		i++;
	}
	return (1);
}

void	destroy_all(t_rules *r, t_philo *ph)
{
	int i;

	(void)ph;
	if (r->forks)
	{
		i = 0;
		while (i < r->n_philo)
		{
			pthread_mutex_destroy(&r->forks[i]);
			i++;
		}
	}
	pthread_mutex_destroy(&r->print_mx);
	pthread_mutex_destroy(&r->death_mutex);
	pthread_mutex_destroy(&r->bz_luca);
	pthread_mutex_destroy(&r->monitor);
	pthread_mutex_destroy(&r->stop_mx);
	gg_free_all(r->gc);
	r->gc = NULL;
}