/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcaylan <rcaylan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 13:17:51 by rbiskin           #+#    #+#             */
/*   Updated: 2025/11/24 14:07:25 by rcaylan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	is_simulation_stopped(t_rules *r)
{
	int	stopped;

	pthread_mutex_lock(&r->stop_mx);
	stopped = r->stop;
	pthread_mutex_unlock(&r->stop_mx);
	return (stopped);
}
