/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gg_init.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbiskin <rbiskin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 08:39:41 by rbiskin           #+#    #+#             */
/*   Updated: 2025/09/24 13:28:56 by rbiskin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/gg_collect.h"

/**
 * Creates and initializes the main data struct for garbage collection.
 * This function MUST be called before using any features of
 * the garbage collector.
 *
 * @return The new main data struct.
 */

 
t_gg_data	*gg_init(void)
{
	t_gg_data	*data;

	data = (t_gg_data *) malloc(sizeof(t_gg_data));
	if (!data)
		return (NULL);
	data->size = 0;
	data->alloc_count = 0;
	data->head = NULL;
	return (data);
}