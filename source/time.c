/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcaylan <rcaylan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 07:32:17 by rbiskin           #+#    #+#             */
/*   Updated: 2025/10/13 13:30:28 by rcaylan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	timestamp_ms(void)
{
	long	ms;

	struct timeval tv;        // contient secondes + microsecondes
	gettimeofday(&tv, NULL);  // remplit tv
	ms = tv.tv_sec * 1000L    // secondes → millisecondes
		+ tv.tv_usec / 1000L; // microsecondes → millisecondes
	return (ms);
}
