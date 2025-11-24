#include "../include/gg_collect.h"

t_gg_data	*gg_init(void)
{
	t_gg_data *data;

	data = (t_gg_data *)malloc(sizeof(t_gg_data));
	data->size = 0;
	data->alloc_count = 0;
	data->head = NULL;
	return (data);
}