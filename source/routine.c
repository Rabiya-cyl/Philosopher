/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbiskin <rbiskin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 09:35:38 by rbiskin           #+#    #+#             */
/*   Updated: 2025/09/24 15:40:18 by rbiskin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void ms_sleep(long ms)
{
    long start = timestamp_ms();
    while (timestamp_ms() - start < ms)
        usleep(100);
}

void *routine(void *arg)
{
    t_philo *p = (t_philo *)arg;

    while (!p->rules->stop)
    {
        print_state(p->rules, p->id, "is thinking");
        
        if (p->id % 2)
        {
            pthread_mutex_lock(p->left_fork);
            pthread_mutex_lock(p->right_fork);
            print_state(p->rules, p->id, "has taken a fork");
            print_state(p->rules, p->id, "has taken a fork");
        }
        else
        {
            pthread_mutex_lock(p->right_fork);
            pthread_mutex_lock(p->left_fork);
            print_state(p->rules, p->id, "has taken a fork");
            print_state(p->rules, p->id, "has taken a fork");
        }

        print_state(p->rules, p->id, "is eating");
        p->last_meal_ms = timestamp_ms();
        ms_sleep(p->rules->t_eat);

        pthread_mutex_unlock(p->left_fork);
        pthread_mutex_unlock(p->right_fork);

        print_state(p->rules, p->id, "is sleeping");
        ms_sleep(p->rules->t_sleep);
    }
    return NULL;
}
