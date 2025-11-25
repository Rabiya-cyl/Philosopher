/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fork_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcaylan <rcaylan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 12:00:00 by rbiskin           #+#    #+#             */
/*   Updated: 2025/11/24 14:05:23 by rcaylan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	take_forks_odd(t_philo *p)
{
	pthread_mutex_lock(p->left_fork);
	if (is_simulation_stopped(p->rules))
	{
		pthread_mutex_unlock(p->left_fork);
		return (0);
	}
	print_state(p->rules, p->id, "has taken a fork");
	pthread_mutex_lock(p->right_fork);
	if (is_simulation_stopped(p->rules))
	{
		pthread_mutex_unlock(p->right_fork);
		pthread_mutex_unlock(p->left_fork);
		return (0);
	}
	print_state(p->rules, p->id, "has taken a fork");
	return (1);
}

int	take_forks_even(t_philo *p)
{
	pthread_mutex_lock(p->right_fork);
	if (is_simulation_stopped(p->rules))
	{
		pthread_mutex_unlock(p->right_fork);
		return (0);
	}
	print_state(p->rules, p->id, "has taken a fork");
	if (p->rules->t_die < (p->rules->t_eat + p->rules->t_sleep) * 1.5)
	{
		pthread_mutex_lock(&p->rules->death_mutex);
		p->last_meal_ms = timestamp_ms();
		pthread_mutex_unlock(&p->rules->death_mutex);
	}
	pthread_mutex_lock(p->left_fork);
	if (is_simulation_stopped(p->rules))
	{
		pthread_mutex_unlock(p->left_fork);
		pthread_mutex_unlock(p->right_fork);
		return (0);
	}
	print_state(p->rules, p->id, "has taken a fork");
	return (1);
}

int	take_forks_last(t_philo *p)
{
	pthread_mutex_lock(p->right_fork);
	if (is_simulation_stopped(p->rules))
	{
		pthread_mutex_unlock(p->right_fork);
		return (0);
	}
	print_state(p->rules, p->id, "has taken a fork");
	pthread_mutex_lock(p->left_fork);
	return (1);
}

int	prend_la_putain_de_fourchette(t_philo *p)
{
	int	critical_case;
	int	is_odd_total;

	critical_case = p->rules->t_die < (p->rules->t_eat + p->rules->t_sleep)
		* 1.5;
	is_odd_total = p->rules->n_philo % 2 == 1;
	if (is_odd_total && critical_case && p->id == p->rules->n_philo)
		return (take_forks_last(p));
	else if (p->id % 2 == 1)
		return (take_forks_odd(p));
	else
		return (take_forks_even(p));
}

void	release_forks(t_philo *p)
{
	pthread_mutex_unlock(p->left_fork);
	pthread_mutex_unlock(p->right_fork);
}
