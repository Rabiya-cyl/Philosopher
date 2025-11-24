#include "philo.h"

long	timestamp_ms(void)
{
	long			ms;
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	ms = tv.tv_sec * 1000L + tv.tv_usec / 1000L;
	return (ms);
}
