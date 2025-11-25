/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcaylan <rcaylan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 06:39:33 by rbiskin           #+#    #+#             */
/*   Updated: 2025/11/24 14:07:26 by rcaylan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	ms_sleep(long ms)
{
	long	start;

	start = timestamp_ms();
	while ((timestamp_ms() - start) < ms)
	{
		if (ms > 10)
			usleep(600);
		else
			usleep(100);
	}
}

static void	initial_delay(t_philo *p)
{
	if (p->rules->n_philo % 2 == 0)
	{
		if (p->id % 2 == 0)
			usleep(500);
	}
	else
	{
		if (p->id % 2 == 0)
			usleep(200);
		else if (p->id > 1)
			usleep(500);
	}
}

static void	thinking_delay(t_rules *r)
{
	if (r->t_die < (r->t_eat + r->t_sleep) * 1.5)
		return ;
	else if (r->t_die < (r->t_eat + r->t_sleep) * 2)
		usleep(200);
	else
		usleep(1000);
}

void	philo_eat(t_philo *p)
{
	pthread_mutex_lock(&p->rules->death_mutex);
	if (p->rules->must_eat > 0 && p->meals >= p->rules->must_eat)
	{
		pthread_mutex_unlock(&p->rules->death_mutex);
		return ;
	}
	pthread_mutex_unlock(&p->rules->death_mutex);
	if (is_simulation_stopped(p->rules))
		return ;
	print_state(p->rules, p->id, "is eating");
	pthread_mutex_lock(&p->rules->death_mutex);
	p->last_meal_ms = timestamp_ms();
	pthread_mutex_unlock(&p->rules->death_mutex);
	ms_sleep(p->rules->t_eat);
	if (is_simulation_stopped(p->rules))
		return ;
	pthread_mutex_lock(&p->rules->death_mutex);
	if (!is_simulation_stopped(p->rules) && (p->rules->must_eat == -1
			|| p->meals < p->rules->must_eat))
		p->meals++;
	pthread_mutex_unlock(&p->rules->death_mutex);
}

void	*routine(void *arg)
{
	t_philo	*p;

	p = (t_philo *)arg;
	initial_delay(p);
	while (!is_simulation_stopped(p->rules))
	{
		print_state(p->rules, p->id, "is thinking");
		thinking_delay(p->rules);
		if (!prend_la_putain_de_fourchette(p))
			break ;
		philo_eat(p);
		release_forks(p);
		if (is_simulation_stopped(p->rules))
			break ;
		print_state(p->rules, p->id, "is sleeping");
		ms_sleep(p->rules->t_sleep);
	}
	return (NULL);
}
