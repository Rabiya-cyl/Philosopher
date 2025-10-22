/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbiskin <rbiskin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 06:39:33 by rbiskin           #+#    #+#             */
/*   Updated: 2025/10/22 15:37:06 by rbiskin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	ms_sleep(long ms)
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

int	is_simulation_stopped(t_rules *r)
{
	int	stopped;

	pthread_mutex_lock(&r->stop_mx);
	stopped = r->stop;
	pthread_mutex_unlock(&r->stop_mx);
	return (stopped);
}

static int	prend_la_putain_de_fourchette(t_philo *p)
{
	int	critical_case;
	int	is_odd_total;

	critical_case = p->rules->t_die < (p->rules->t_eat + p->rules->t_sleep)
		* 1.5;
	is_odd_total = p->rules->n_philo % 2 == 1;
	if (is_odd_total && critical_case && p->id == p->rules->n_philo)
	{
		pthread_mutex_lock(p->right_fork);
		if (is_simulation_stopped(p->rules))
		{
			pthread_mutex_unlock(p->right_fork);
			return (0);
		}
		print_state(p->rules, p->id, "has taken a fork");
		pthread_mutex_lock(p->left_fork);
	}
	else if (p->id % 2 == 1)
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
	}
	else
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
	}
	return (1);
}

static void	release_forks(t_philo *p)
{
	pthread_mutex_unlock(p->left_fork);
	pthread_mutex_unlock(p->right_fork);
}

static void	philo_eat(t_philo *p)
{
	int	i;
	int	philo_finished;

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
	{
		p->meals++;
		if (p->rules->must_eat > 0)
		{
			philo_finished = 0;
			i = 0;
			while (i < p->rules->n_philo)
			{
				while (i < p->rules->n_philo)
				{
					if (p->rules->philosophers[i].meals >= p->rules->must_eat)
						philo_finished++;
					i++;
				}
				if (philo_finished == p->rules->n_philo)
				{
					pthread_mutex_lock(&p->rules->stop_mx);
					p->rules->stop = 1;
					pthread_mutex_unlock(&p->rules->stop_mx);
				}
			}
		}
	}
	pthread_mutex_unlock(&p->rules->death_mutex);
}

void	*routine(void *arg)
{
	t_philo	*p;

	p = (t_philo *)arg;
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
	while (!is_simulation_stopped(p->rules))
	{
		print_state(p->rules, p->id, "is thinking");
		if (p->rules->t_die < (p->rules->t_eat + p->rules->t_sleep) * 1.5)
		{
		}
		else if (p->rules->t_die < (p->rules->t_eat + p->rules->t_sleep) * 2)
		{
			usleep(200);
		}
		else
		{
			usleep(1000);
		}
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

int	check_death(t_rules *r, t_philo *ph)
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

int	check_meals_done(t_rules *r)
{
	int	philo_finished;
	int	i;

	if (r->must_eat < 0)
		return (0);
	if (is_simulation_stopped(r))
		return (1);
	pthread_mutex_lock(&r->death_mutex);
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

void	*monitor_routine(void *arg)
{
	t_rules	*r;
	
	r = (t_rules *)arg;
	while (!la_putain_de_simulation_qui_se_termine(r))
	{
		if (regarde_si_le_fils_de_pute_est_pas_mort(r, r->philosophers)
			|| nbr_de_repas(r, r->philosophers))
		{
			break ;
		}
		if (r->t_die < (r->t_eat + r->t_sleep) * 1.5)
		{
			usleep(100);
		}
		else
		{
			usleep(300);
		}
	}
	return (NULL);
}

int	regarde_si_le_fils_de_pute_est_pas_mort(t_rules *r, t_philo *ph)
{
	return (check_death(r, ph));
}

int	nbr_de_repas(t_rules *r, t_philo *ph)
{
	int	i;
	int	all_ate_enough;
	int	meals;

	i = 0;
	all_ate_enough = 1;
	if (r->must_eat == -1)
		return (0);
	while (i < r->n_philo)
	{
		pthread_mutex_lock(&r->death_mutex);
		meals = ph[i].meals;
		pthread_mutex_unlock(&r->death_mutex);
		if (meals < r->must_eat)
		{
			all_ate_enough = 0;
			break ;
		}
		i++;
	}
	if (all_ate_enough)
	{
		pthread_mutex_lock(&r->stop_mx);
		r->stop = 1;
		pthread_mutex_unlock(&r->stop_mx);
		return (1);
	}
	return (0);
}

int	la_putain_de_simulation_qui_se_termine(t_rules *r)
{
	return (is_simulation_stopped(r));
}
