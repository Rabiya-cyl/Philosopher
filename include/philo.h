/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbiskin <rbiskin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/21 12:33:45 by rbiskin           #+#    #+#             */
/*   Updated: 2025/10/21 12:33:46 by rbiskin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include "gg_collect.h"
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_gg_data	t_gc;

typedef struct s_rules
{
	int						n_philo;
	long					t_die;
	long					t_eat;
	long					t_sleep;
	int						must_eat;
	long					start_ms;
	int						stop;

	t_gc					*gc;
	pthread_t				monitor_thread;
	struct s_philo			*philosophers;

	pthread_mutex_t			print_mx;
	pthread_mutex_t			*forks;

	pthread_mutex_t			death_mutex;
	pthread_mutex_t			bz_luca;
	pthread_mutex_t			monitor;
	pthread_mutex_t			stop_mx;
}							t_rules;

typedef struct s_philo
{
	int						id;
	long					last_meal_ms;
	int						meals;
	pthread_t				thread;
	t_rules					*rules;
	pthread_mutex_t			*left_fork;
	pthread_mutex_t			*right_fork;
}							t_philo;
int							parse_args(int ac, char **av, t_rules *r);
int							init_all(t_rules *r, t_philo **ph);
void						destroy_all(t_rules *r, t_philo *ph);
long						timestamp_ms(void);
void						print_state(t_rules *r, int id, const char *msg);
void						*routine(void *arg);

void						*monitor_routine(void *arg);
int							regarde_si_le_fils_de_pute_est_pas_mort(t_rules *r,
								t_philo *ph);
int							nbr_de_repas(t_rules *r, t_philo *ph);
int							la_putain_de_simulation_qui_se_termine(t_rules *r);

#endif