/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbiskin <rbiskin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 09:35:38 by rbiskin           #+#    #+#             */
/*   Updated: 2025/09/23 10:27:45 by rbiskin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void *routine(void *arg)
{
    t_philo *p;
    
    p = (t_philo *)arg;
    print_state(p->rules, p->id, "is thinkin");
    usleep(50 * 1000);  // attend 0.05 
    print_state(p->rules, p->id,"is ready");
    return(0);
}