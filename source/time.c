/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbiskin <rbiskin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 07:32:17 by rbiskin           #+#    #+#             */
/*   Updated: 2025/09/24 15:30:18 by rbiskin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	timestamp_ms(void)
{
       struct timeval	tv;                 // contient secondes + microsecondes
	long			ms;

	gettimeofday(&tv, NULL);            // remplit tv
	ms = tv.tv_sec * 1000L              // secondes → millisecondes
		+ tv.tv_usec / 1000L;           // microsecondes → millisecondes
	return (ms);
}
