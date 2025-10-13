/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcaylan <rcaylan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 07:32:23 by rbiskin           #+#    #+#             */
/*   Updated: 2025/10/13 13:29:58 by rcaylan          ###   ########.fr       */
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
	int must_eat; /* -1 si non fourni */
	long					start_ms;
	int						stop;

	t_gc *gc;                     // handle
	pthread_t monitor_thread;     /* monitor thread */
	struct s_philo *philosophers; /* pointeur vers le tableau des philosophes */

	pthread_mutex_t print_mx; /* mutex pour printf */
	pthread_mutex_t *forks;   /* tableau de mutex fourchettes */

	pthread_mutex_t			death_mutex;
	// mutex pour la salope de mort du philo de mes couilles
	pthread_mutex_t bz_luca; // le nombre de repas
	pthread_mutex_t			monitor;
	// pour la regarder l enorme fiak a sudinaz je rajoute la grosse chienne.
	pthread_mutex_t			stop_mx;
}							t_rules;

typedef struct s_philo
//    la simulation prématurément alors que le philosophe mange encore
{
	int id; /* 1..n */
	long					last_meal_ms;
	int						meals;
	pthread_t				thread;
	t_rules					*rules;
	pthread_mutex_t			*left_fork;
	pthread_mutex_t			*right_fork;
}							t_philo;

/* prototypes */
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