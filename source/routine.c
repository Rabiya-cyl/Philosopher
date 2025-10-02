#include "philo.h"

static void ft_usleep(long ms)
{
    long start = timestamp_ms();
    while ((timestamp_ms() - start) < ms)
        usleep(500);
}

int is_simulation_stopped(t_rules *r)
{
    int stopped;
    pthread_mutex_lock(&r->stop_mx);
    stopped = r->stop;
    pthread_mutex_unlock(&r->stop_mx);
    return (stopped);
}

static int take_forks(t_philo *p)
{
    pthread_mutex_lock(p->left_fork);
    if (is_simulation_stopped(p->rules))
    {
        pthread_mutex_unlock(p->left_fork);
        return (0);
    }
    print_state(p->rules, p->id, "has taken a fork");
    
    pthread_mutex_lock(p->right_fork);
    if (is_simulation_stopped(p->rules))
    {
        pthread_mutex_unlock(p->right_fork);
        pthread_mutex_unlock(p->left_fork);
        return (0);
    }
    print_state(p->rules, p->id, "has taken a fork");
    
    return (1);
}

static void release_forks(t_philo *p)
{
    pthread_mutex_unlock(p->left_fork);
    pthread_mutex_unlock(p->right_fork);
}

static void eat_sleep_routine(t_philo *p)
{
    // Take forks
    if (!take_forks(p))
        return;
    
    // Eat
    print_state(p->rules, p->id, "is eating");
    pthread_mutex_lock(&p->rules->meal_mutex);
    p->last_meal_ms = timestamp_ms();
    pthread_mutex_unlock(&p->rules->meal_mutex);
    ft_usleep(p->rules->t_eat);
    
    // Update meal count after eating
    if (!is_simulation_stopped(p->rules))
    {
        pthread_mutex_lock(&p->rules->meal_mutex);
        p->meals++;
        pthread_mutex_unlock(&p->rules->meal_mutex);
    }
    
    // Sleep
    print_state(p->rules, p->id, "is sleeping");
    release_forks(p);
    ft_usleep(p->rules->t_sleep);
}

static void think_routine(t_philo *p, int silent)
{
    long time_to_think;
    
    pthread_mutex_lock(&p->rules->meal_mutex);
    time_to_think = (p->rules->t_die - (timestamp_ms() - p->last_meal_ms) - p->rules->t_eat) / 2;
    pthread_mutex_unlock(&p->rules->meal_mutex);
    
    if (time_to_think < 0)
        time_to_think = 0;
    if (time_to_think == 0 && silent == 1)
        time_to_think = 1;
    if (time_to_think > 600)
        time_to_think = 200;
    
    if (silent == 0)
        print_state(p->rules, p->id, "is thinking");
    
    if (time_to_think > 0)
        ft_usleep(time_to_think);
}

static void *lone_philo_routine(t_philo *p)
{
    pthread_mutex_lock(p->left_fork);
    print_state(p->rules, p->id, "has taken a fork");
    ft_usleep(p->rules->t_die);
    print_state(p->rules, p->id, "died");
    pthread_mutex_unlock(p->left_fork);
    return (NULL);
}

void *routine(void *arg)
{
    t_philo *p = (t_philo *)arg;

    // Initialize last meal time
    pthread_mutex_lock(&p->rules->meal_mutex);
    p->last_meal_ms = p->rules->start_ms;
    pthread_mutex_unlock(&p->rules->meal_mutex);

    // Handle single philosopher case
    if (p->rules->n_philo == 1)
        return (lone_philo_routine(p));

    // Stagger start: even philosophers think first to avoid deadlock
    if (p->id % 2 == 0)
        think_routine(p, 1);

    while (!is_simulation_stopped(p->rules))
    {
        eat_sleep_routine(p);
        if (is_simulation_stopped(p->rules))
            break;
        think_routine(p, 0);
    }
    return (NULL);
}

int check_death(t_rules *r, t_philo *ph)
{
    int i = 0;
    
    while (i < r->n_philo)
    {
        pthread_mutex_lock(&r->meal_mutex);
        long time_since_meal = timestamp_ms() - ph[i].last_meal_ms;
        pthread_mutex_unlock(&r->meal_mutex);
        
        if (time_since_meal > r->t_die)
        {
            pthread_mutex_lock(&r->stop_mx);
            if (!r->stop)
            {
                r->stop = 1;
                pthread_mutex_unlock(&r->stop_mx);
                
                // Print death message immediately
                pthread_mutex_lock(&r->print_mx);
                printf("%ld %d died\n", timestamp_ms() - r->start_ms, ph[i].id);
                pthread_mutex_unlock(&r->print_mx);
                return (1);
            }
            else
            {
                pthread_mutex_unlock(&r->stop_mx);
                return (1);
            }
        }
        i++;
    }
    return (0);
}

int check_meals_done(t_rules *r, t_philo *ph)
{
    int i = 0;
    
    if (r->must_eat == -1)
        return (0);
        
    pthread_mutex_lock(&r->meal_mutex);
    i = 0;
    while (i < r->n_philo)
    {
        if (ph[i].meals < r->must_eat)
        {
            pthread_mutex_unlock(&r->meal_mutex);
            return (0);
        }
        i++;
    }
    pthread_mutex_unlock(&r->meal_mutex);
    
    // All philosophers have eaten enough
    pthread_mutex_lock(&r->stop_mx);
    if (!r->stop)
    {
        r->stop = 1;
        pthread_mutex_unlock(&r->stop_mx);
        return (1);
    }
    else
    {
        pthread_mutex_unlock(&r->stop_mx);
        return (1);
    }
}

void *monitor_routine(void *arg)
{
    t_rules *r = (t_rules *)arg;
    
    // Don't monitor if there's only one philosopher (handled in routine)
    if (r->n_philo == 1)
        return (NULL);
        
    while (!is_simulation_stopped(r))
    {
        if (check_death(r, r->philosophers))
            break;
        if (check_meals_done(r, r->philosophers))
            break;
        usleep(500);
    }
    return (NULL);
}
