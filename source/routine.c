#include "philo.h"

static void ms_sleep(long ms)
{
    long start = timestamp_ms();
    while ((timestamp_ms() - start) < ms)
    {
        if (ms > 10)
            usleep(500);
        else
            usleep(100);
    }
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
    pthread_mutex_t *first_fork;
    pthread_mutex_t *second_fork;
    
    // Stratégie anti-deadlock améliorée : toujours prendre les forks dans le même ordre
    // En fonction de l'adresse mémoire pour éviter les deadlocks circulaires
    if (p->left_fork < p->right_fork)
    {
        first_fork = p->left_fork;
        second_fork = p->right_fork;
    }
    else
    {
        first_fork = p->right_fork;
        second_fork = p->left_fork;
    }
    
    pthread_mutex_lock(first_fork);
    if (is_simulation_stopped(p->rules))
    {
        pthread_mutex_unlock(first_fork);
        return (0);
    }
    print_state(p->rules, p->id, "has taken a fork");
    
    pthread_mutex_lock(second_fork);
    if (is_simulation_stopped(p->rules))
    {
        pthread_mutex_unlock(second_fork);
        pthread_mutex_unlock(first_fork);
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

static void philo_eat(t_philo *p)
{
    print_state(p->rules, p->id, "is eating");
    
    // Mettre à jour le temps du dernier repas ET le compteur de repas sous le même mutex
    pthread_mutex_lock(&p->rules->meal_mutex);
    p->last_meal_ms = timestamp_ms();
    p->meals++;
    pthread_mutex_unlock(&p->rules->meal_mutex);
    
    ms_sleep(p->rules->t_eat);
}

void *routine(void *arg)
{
    t_philo *p = (t_philo *)arg;

    // Décalage intelligent pour éviter la contention initiale
    // Les philosophes pairs attendent un peu avant de commencer
    if (p->id % 2 == 0)
    {
        print_state(p->rules, p->id, "is thinking");
        ms_sleep(p->rules->t_eat / 2);
    }

    while (!is_simulation_stopped(p->rules))
    {
        print_state(p->rules, p->id, "is thinking");
        
        if (!take_forks(p))
            break;
            
        philo_eat(p);
        
        release_forks(p);
        
        if (is_simulation_stopped(p->rules))
            break;
            
        print_state(p->rules, p->id, "is sleeping");
        ms_sleep(p->rules->t_sleep);
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
                pthread_mutex_lock(&r->print_mx);
                printf("%ld %d died\n", timestamp_ms() - r->start_ms, ph[i].id);
                pthread_mutex_unlock(&r->print_mx);
            }
            else
            {
                pthread_mutex_unlock(&r->stop_mx);
            }
            return (1);
        }
        i++;
    }
    return (0);
}

int check_meals_done(t_rules *r, t_philo *ph)
{
    int i = 0;
    int all_ate_enough = 1;
    
    if (r->must_eat == -1)
        return (0);
        
    while (i < r->n_philo)
    {
        pthread_mutex_lock(&r->meal_mutex);
        int meals = ph[i].meals;
        pthread_mutex_unlock(&r->meal_mutex);
        
        if (meals < r->must_eat)
        {
            all_ate_enough = 0;
        }
        i++;
    }
    
    if (all_ate_enough)
    {
        pthread_mutex_lock(&r->stop_mx);
        r->stop = 1;
        pthread_mutex_unlock(&r->stop_mx);
        return (1);
    }
    return (0);
}

void *monitor_routine(void *arg)
{
    t_rules *r = (t_rules *)arg;
    
    while (!is_simulation_finished(r))
    {
        if (check_philosopher_death(r, r->philosophers) || check_meals_complete(r, r->philosophers))
            break;
        usleep(500); // Vérification plus fréquente pour une détection rapide
    }
    return (NULL);
}

int check_philosopher_death(t_rules *r, t_philo *ph)
{
    return check_death(r, ph);
}

int check_meals_complete(t_rules *r, t_philo *ph)
{
    return check_meals_done(r, ph);
}

int is_simulation_finished(t_rules *r)
{
    return is_simulation_stopped(r);
}
