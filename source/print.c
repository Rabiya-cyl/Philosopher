/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcaylan <rcaylan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 08:26:35 by rbiskin           #+#    #+#             */
/*   Updated: 2025/10/13 11:06:46 by rcaylan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
Si deux philosophes parlent en même temps et bah  on n’entend rien (mélange).

Donc, avant de parler, ils doivent prendre le micro (mutex lock).

Quand ils ont fini, ils rendent le micro (mutex unlock).

Le message contient : le temps écoulé depuis le début, l’ID du philosophe,
	et l’action.
*/

#include "philo.h"

void	print_state(t_rules *r, int id, const char *msg)
{
	long	now;

	pthread_mutex_lock(&r->print_mx);    // je prend le micro
	now = timestamp_ms() - r->start_ms;  /// temps depuis le debut
	printf("%ld %d %s\n", now, id, msg); // message
	pthread_mutex_unlock(&r->print_mx);  // je rend le micro
}
