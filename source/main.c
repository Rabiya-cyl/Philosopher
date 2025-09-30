/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alanty <alanty@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 08:34:21 by rbiskin           #+#    #+#             */
/*   Updated: 2025/09/30 11:26:34 by alanty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"


// int	main(void) // pour time.c
// {
// 	long t1 = timestamp_ms();
// 	usleep(200 * 1000);  // pause 200ms
// 	long t2 = timestamp_ms();
// 	printf("Delta = %ld ms\n", t2 - t1);
// 	return 0;
// }

// int	main(int ac, char **av)
// {
// 	t_rules	r;

// 	(void)ac; (void)av;
// 	r.stop = 0;
// 	r.start_ms = timestamp_ms();
// 	pthread_mutex_init(&r.print_mx, NULL);

// 	print_state(&r, 1, "is here");
// 	usleep(100 * 1000);
// 	print_state(&r, 2, "is thinking");

// 	pthread_mutex_destroy(&r.print_mx);
// 	return (0);
// }

#include "philo.h"



// si un seule philo
static int si_un_seule_fils_de_pute(t_rules *r)
{
    print_state(r, 1, "has token a fork");
    usleep(r->t_die * 1000);
    print_state()(r, 1, "died");
    return (0);
}



int main(int ac, char **av)
{
    t_rules r;
    t_philo *ph = NULL;

    if (!parse_args(ac, av, &r))
        return (printf("Error: bad gay\n"), 1);

    r.stop = 0;
    r.start_ms = timestamp_ms();

    if (!init_all(&r, &ph))
        return (printf("Error: init\n"), 1);

    i = 0;
    while (i < r.n_philo)
    {
        if (pthread_create(&ph[i].thread, NULL, routine, &ph[i]) != 0)
            return (printf("Error: thread\n"), destroy_all(&r, ph), 1);
        i++;
    }
    i = 0;
    while (i < r.n_philo)
        pthread_join(ph[i++].thread, NULL);

    destroy_all(&r, ph);
    return (0);
}
