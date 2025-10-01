/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alanty <alanty@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 08:34:21 by rbiskin           #+#    #+#             */
/*   Updated: 2025/09/30 12:03:12 by alanty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"


#include "philo.h"



// si un seule philo
static int handle_single_philosopher(t_rules *r)
{
    print_state(r, 1, "has taken a fork");
    usleep(r->t_die * 1000);
    print_state(r, 1, "died");
    return (0);
}



int main(int ac, char **av)
{
    t_rules r;
    t_philo *ph = NULL;

    if (!parse_args(ac, av, &r))
        return (printf("Error: invalid arguments\n"), 1);

    r.stop = 0;
    r.start_ms = timestamp_ms();

    if (!init_all(&r, &ph))
        return (printf("Error: initialization failed\n"), 1);

    /* Cas spécial : un seul philosophe */
    if (r.n_philo == 1)
        return (handle_single_philosopher(&r), destroy_all(&r, ph), 0);

    /* Créer les threads des philosophes */
    int i = 0;
    while (i < r.n_philo)
    {
        if (pthread_create(&ph[i].thread, NULL, routine, &ph[i]) != 0)
            return (printf("Error: failed to create philosopher thread %d\n", i + 1), destroy_all(&r, ph), 1);
        i++;
    }

    /* Créer le thread monitor */
    if (pthread_create(&r.monitor_thread, NULL, monitor_routine, &r) != 0)
    {
        printf("Error: failed to create monitor thread\n");
        pthread_mutex_lock(&r.stop_mx);
        r.stop = 1;
        pthread_mutex_unlock(&r.stop_mx);
        i = 0;
        while (i < r.n_philo)
            pthread_join(ph[i++].thread, NULL);
        return (destroy_all(&r, ph), 1);
    }

    /* Attendre que tous les threads se terminent */
    i = 0;
    while (i < r.n_philo)
        pthread_join(ph[i++].thread, NULL);
    pthread_join(r.monitor_thread, NULL);

    destroy_all(&r, ph);
    return (0);
}
