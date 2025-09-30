/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbiskin <rbiskin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 08:47:42 by rbiskin           #+#    #+#             */
/*   Updated: 2025/09/24 13:46:48 by rbiskin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "philo.h"

// static int init_mutexes(t_rules *r)
// {
//     if (pthread_mutex_init(&r->print_mx, NULL) != 0)
//         return (0);

//     r->forks = (pthread_mutex_t *)gg_malloc(r->gc, sizeof(pthread_mutex_t) * r->n_philo);
//     if (!r->forks)
//         return (0);

//     for (int i = 0; i < r->n_philo; i++)
//         if (pthread_mutex_init(&r->forks[i], NULL) != 0)
//             return (0);
//     return (1);
// }

// int init_all(t_rules *r, t_philo **ph)
// {
//     r->gc = gg_init();
//     if (!r->gc)
//         return (0);

//     if (!init_mutexes(r))
//         return (0);


//     *ph = (t_philo *)gg_malloc(r->gc, sizeof(t_philo) * r->n_philo);
//     if (!*ph)
//         return (0);

//     for (int i = 0; i < r->n_philo; i++)
//     {
//         (*ph)[i].id = i + 1;
//         (*ph)[i].meals = 0;
//         (*ph)[i].last_meal_ms = r->start_ms;
//         (*ph)[i].rules = r;
//         (*ph)[i].left_fork  = &r->forks[i];
//         (*ph)[i].right_fork = &r->forks[(i + 1) % r->n_philo];
//     }
//     return (1);
// }

// void destroy_all(t_rules *r, t_philo *ph)
// {
//     (void)ph; /* rien à free à la main : GG s’en occupe */

//     /* IMPORTANT : détruire les mutex AVANT gg_free_all */
//     if (r->forks)
//         for (int i = 0; i < r->n_philo; i++)
//             pthread_mutex_destroy(&r->forks[i]);
//     pthread_mutex_destroy(&r->print_mx);

//     gg_free_all(r->gc);
//     r->gc = NULL;
// }

#include "philo.h"

static int init_mutexes(t_rules *r)
{
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

    r->forks = (pthread_mutex_t *)gg_malloc(r->gc, sizeof(pthread_mutex_t) * r->n_philo);
    if (!r->forks)
        return (0);

    for (int i = 0; i < r->n_philo; i++)
        if (pthread_mutex_init(&r->forks[i], NULL) != 0)
            return (0);
    return (1);
}

int init_all(t_rules *r, t_philo **ph)
{
    r->gc = gg_init();                
    if (!r->gc)
        return (0);

    if (!init_mutexes(r))
        return (0);

    *ph = (t_philo *)gg_malloc(r->gc, sizeof(t_philo) * r->n_philo); 
    if (!*ph)
        return (0);

    r->philosophers = *ph;  /* Assigner le pointeur dans la structure rules */

    for (int i = 0; i < r->n_philo; i++)
    {
        (*ph)[i].id = i + 1;
        (*ph)[i].meals = 0;
        (*ph)[i].last_meal_ms = r->start_ms;
        (*ph)[i].rules = r;
        (*ph)[i].left_fork  = &r->forks[i];
        (*ph)[i].right_fork = &r->forks[(i + 1) % r->n_philo];
    }
    return (1);
}

void destroy_all(t_rules *r, t_philo *ph)
{
    (void)ph; 

    if (r->forks)
        for (int i = 0; i < r->n_philo; i++)
            pthread_mutex_destroy(&r->forks[i]);
    pthread_mutex_destroy(&r->print_mx);
    pthread_mutex_destroy(&r->death_mutex);
    pthread_mutex_destroy(&r->bz_luca);
    pthread_mutex_destroy(&r->monitor);
    pthread_mutex_destroy(&r->stop_mx);

    gg_free_all(r->gc);
    r->gc = NULL;
}
