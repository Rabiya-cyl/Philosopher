/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbiskin <rbiskin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 07:32:23 by rbiskin           #+#    #+#             */
/*   Updated: 2025/09/19 09:36:01 by rbiskin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef PHILO_H
# define PHILO_H

/* -------------------- Librairie -------------------- */

# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <pthread.h>
# include <sys/time.h>

/* -------------------- structures -------------------- */
typedef struct s_rules
{
	int				n_philo;
	long			t_die;
	long			t_eat;
	long			t_sleep;
	int				must_eat;     /* -1 si non fourni */
	long			start_ms;     /* timestamp de départ en ms */
	int				stop;         /* 1 = on arrête */
	pthread_mutex_t	print_mx;     /* protège l'affichage, print_mx doit être un pthread_mutex_t et pas un char pck 
                                    si deux philosophes font printf en même temps bah les phrases se mélangent à l’écran */
	pthread_mutex_t	*forks;       /* tableau de mutex (fourchettes) */
}	t_rules;

typedef struct s_philo
{
	int				id;            /* 1..n */
	long			last_meal_ms;  /* moment du dernier repas (ms) */
	int				meals;         /* nb de repas pris */
	pthread_t		thread;        /* son thread */
	t_rules			*rules;        /* lien vers les règles */
	pthread_mutex_t	*left_fork;    /* &rules->forks[i] */
	pthread_mutex_t	*right_fork;   /* &rules->forks[(i + 1) % n_philo] */
}	t_philo;

/* -------------------- prototypes -------------------- */
int		parse_args(int ac, char **av, t_rules *r);  /* parsing.c */

long	timestamp_ms(void);                          /* time.c    */

void	print_state(t_rules *r, int id, const char *msg); /* print.c */

int		init_all(t_rules *r, t_philo **ph);         /* init.c   */
void	destroy_all(t_rules *r, t_philo *ph);

void	*routine(void *arg);                         /* routine.c */

#endif
