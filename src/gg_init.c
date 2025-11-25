/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gg_init.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcaylan <rcaylan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 12:00:00 by rbiskin           #+#    #+#             */
/*   Updated: 2025/11/24 14:52:00 by rcaylan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/gg_collect.h"

t_gg_data	*gg_init(void)
{
	t_gg_data	*data;

	data = (t_gg_data *)malloc(sizeof(t_gg_data));
	if (!data)
		return (NULL);
	data->size = 0;
	data->alloc_count = 0;
	data->head = NULL;
	return (data);
}
