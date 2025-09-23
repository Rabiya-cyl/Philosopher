/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbiskin <rbiskin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 07:32:17 by rbiskin           #+#    #+#             */
/*   Updated: 2025/09/23 09:21:17 by rbiskin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <sys/time.h>

long	timestamp_ms(void)
{
       struct timeval	tv;                 // contient secondes + microsecondes
	long			ms;

	gettimeofday(&tv, NULL);            // remplit tv
	ms = tv.tv_sec * 1000L              // secondes → millisecondes
		+ tv.tv_usec / 1000L;           // microsecondes → millisecondes
	return (ms);
}

/*
int	main(void)
{
	long t1 = timestamp_ms();
	usleep(200 * 1000);  // pause 200ms
	long t2 = timestamp_ms();
	printf("Delta = %ld ms\n", t2 - t1);
	return 0;
}
*/