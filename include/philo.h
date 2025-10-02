/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alanty <alanty@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 07:32:23 by rbiskin           #+#    #+#             */
/*   Updated: 2025/10/01 15:51:55 by alanty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef PHILO_H
# define PHILO_H

# include "gg_collect.h"    
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <pthread.h>
# include <sys/time.h>

typedef struct s_gg_data t_gc;

typedef struct s_rules
{
    int              n_philo;
    long             t_die;
    long             t_eat;
    long             t_sleep;
    int              must_eat;     /* -1 si non fourni */
    long             start_ms;
    int              stop;

    t_gc            *gc;          //handle 
    pthread_t        monitor_thread;  /* monitor thread */
    struct s_philo  *philosophers;    /* pointeur vers le tableau des philosophes */

    pthread_mutex_t  print_mx;     /* mutex pour printf */
    pthread_mutex_t *forks;        /* tableau de mutex fourchettes */

    pthread_mutex_t death_mutex; // mutex pour la mort du philosophe
    pthread_mutex_t meal_mutex; // mutex pour le nombre de repas
    pthread_mutex_t monitor_mutex; // mutex pour le monitoring
    pthread_mutex_t stop_mx;
}   t_rules;

typedef struct s_philo
{
    int              id;           /* 1..n */
    long             last_meal_ms;
    int              meals;
    pthread_t        thread;
    t_rules         *rules;
    pthread_mutex_t *left_fork;
    pthread_mutex_t *right_fork;
}   t_philo;

/* prototypes */
int   parse_args(int ac, char **av, t_rules *r);
int   init_all(t_rules *r, t_philo **ph);
void  destroy_all(t_rules *r, t_philo *ph);
long  timestamp_ms(void);
void  print_state(t_rules *r, int id, const char *msg);
void *routine(void *arg);
int   is_simulation_stopped(t_rules *r);

void *monitor_routine(void *arg);
int   check_philosopher_death(t_rules *r, t_philo *ph);
int   check_meals_complete(t_rules *r, t_philo *ph);
int   is_simulation_finished(t_rules *r);


#endif
