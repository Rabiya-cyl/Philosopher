/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcaylan <rcaylan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 12:00:00 by rbiskin           #+#    #+#             */
/*   Updated: 2025/11/24 14:05:23 by rcaylan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	regarde_si_le_fils_de_pute_est_pas_mort(t_rules *r, t_philo *ph)
{
	int		i;
	long	time_since_meal;

	i = 0;
	while (i < r->n_philo)
	{
		pthread_mutex_lock(&r->death_mutex);
		time_since_meal = timestamp_ms() - ph[i].last_meal_ms;
		pthread_mutex_unlock(&r->death_mutex);
		if (time_since_meal >= r->t_die)
		{
			print_state(r, ph[i].id, "died");
			pthread_mutex_lock(&r->stop_mx);
			r->stop = 1;
			pthread_mutex_unlock(&r->stop_mx);
			return (1);
		}
		i++;
	}
	return (0);
}

static int	count_finished(t_rules *r)
{
	int	philo_finished;
	int	i;

	philo_finished = 0;
	i = 0;
	while (i < r->n_philo)
	{
		if (r->philosophers[i].meals >= r->must_eat)
			philo_finished++;
		if (r->philosophers[i].meals > r->must_eat)
			r->philosophers[i].meals = r->must_eat;
		i++;
	}
	return (philo_finished);
}

int	nbr_de_repas(t_rules *r, t_philo *ph)
{
	int	philo_finished;

	(void)ph;
	if (r->must_eat < 0)
		return (0);
	if (is_simulation_stopped(r))
		return (1);
	pthread_mutex_lock(&r->death_mutex);
	philo_finished = count_finished(r);
	if (philo_finished == r->n_philo)
	{
		pthread_mutex_lock(&r->stop_mx);
		r->stop = 1;
		pthread_mutex_unlock(&r->stop_mx);
		pthread_mutex_unlock(&r->death_mutex);
		return (1);
	}
	pthread_mutex_unlock(&r->death_mutex);
	return (0);
}

int	la_putain_de_simulation_qui_se_termine(t_rules *r)
{
	return (is_simulation_stopped(r));
}

void	*monitor_routine(void *arg)
{
	t_rules	*r;

	r = (t_rules *)arg;
	while (!la_putain_de_simulation_qui_se_termine(r))
	{
		if (regarde_si_le_fils_de_pute_est_pas_mort(r, r->philosophers)
			|| nbr_de_repas(r, r->philosophers))
			break ;
		if (r->t_die < (r->t_eat + r->t_sleep) * 1.5)
			usleep(100);
		else
			usleep(300);
	}
	return (NULL);
}
