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
    int stopped;// pour l etat du philo
    pthread_mutex_lock(&r->stop_mx);
    stopped = r->stop;
    pthread_mutex_unlock(&r->stop_mx);
    return (stopped);
}




static int prend_la_putain_de_fourchette(t_philo *p)
{
    if (p->id % 2 == 1) //impaire
    {
        // prend la fourchette de gauche en 
        pthread_mutex_lock(p->left_fork);
        /* Vérifier si la simulation s'est arrêtée entre temps */
        if (is_simulation_stopped(p->rules))
        {
            pthread_mutex_unlock(p->left_fork); /* Libérer la fourchette */
            return (0); /* Échec - simulation arrêtée */
        }
        print_state(p->rules, p->id, "has taken a fork"); /* Annoncer la prise de fourchette */
        
        /* Prendre ensuite la fourchette de droite */
        pthread_mutex_lock(p->right_fork);
        /* Vérifier encore si la simulation s'est arrêtée */
        if (is_simulation_stopped(p->rules))
        {
            pthread_mutex_unlock(p->right_fork); /* Libérer les deux fourchettes */
            pthread_mutex_unlock(p->left_fork);
            return (0); /* Échec - simulation arrêtée */
        }
        print_state(p->rules, p->id, "has taken a fork"); /* Annoncer la deuxième fourchette */
    }
    else /* Philosophes avec un ID pair */
    {
        /* Prendre d'abord la fourchette de droite (ordre inverse pour éviter deadlock) */
        pthread_mutex_lock(p->right_fork);
        /* Vérifier si la simulation s'est arrêtée entre temps */
        if (is_simulation_stopped(p->rules))
        {
            pthread_mutex_unlock(p->right_fork); /* Libérer la fourchette */
            return (0); /* Échec - simulation arrêtée */
        }
        print_state(p->rules, p->id, "has taken a fork"); /* Annoncer la prise de fourchette */
        
        /* Prendre ensuite la fourchette de gauche */
        pthread_mutex_lock(p->left_fork);
        /* Vérifier encore si la simulation s'est arrêtée */
        if (is_simulation_stopped(p->rules))
        {
            pthread_mutex_unlock(p->left_fork); /* Libérer les deux fourchettes */
            pthread_mutex_unlock(p->right_fork);
            return (0); /* Échec - simulation arrêtée */
        }
        print_state(p->rules, p->id, "has taken a fork"); /* Annoncer la deuxième fourchette */
    }
    return (1); /* Succès - les deux fourchettes ont été prises */
}

static void release_forks(t_philo *p)
{
    /* Libérer les deux fourchettes (l'ordre n'est pas important ici) */
    pthread_mutex_unlock(p->left_fork);  /* Libérer la fourchette de gauche */
    pthread_mutex_unlock(p->right_fork); /* Libérer la fourchette de droite */
}

static void philo_eat(t_philo *p)
{
    print_state(p->rules, p->id, "is eating"); /* Annoncer que le philosophe mange */
    
    /* Mettre à jour les données du philosophe de manière thread-safe */
    pthread_mutex_lock(&p->rules->death_mutex);
    p->last_meal_ms = timestamp_ms(); /* Enregistrer l'heure du repas actuel */
    p->meals++; /* Incrémenter le nombre de repas pris */
    pthread_mutex_unlock(&p->rules->death_mutex);
    
    ms_sleep(p->rules->t_eat); /* Simuler le temps de manger */
}

void *routine(void *arg)
{
    t_philo *p = (t_philo *)arg; /* Récupérer les données du philosophe */

    /* Décaler le début pour les philosophes pairs pour éviter la contention */
    if (p->id % 2 == 0)
        usleep(1000); /* Attendre 1ms pour désynchroniser */

    /* Boucle principale du philosophe */
    while (!is_simulation_stopped(p->rules))
    {
        /* PHASE 1: RÉFLEXION */
        print_state(p->rules, p->id, "is thinking");
        
        /* PHASE 2: PRISE DES FOURCHETTES */
        if (!prend_la_putain_de_fourchette(p)) /* Essayer de prendre les fourchettes */
            break; /* Si échec (simulation arrêtée), sortir de la boucle */
            
        /* PHASE 3: MANGER */
        philo_eat(p); /* Manger avec les fourchettes */
        
        /* PHASE 4: LIBÉRATION DES FOURCHETTES */
        release_forks(p); /* Rendre les fourchettes */
        
        /* Vérifier si la simulation s'est arrêtée après avoir mangé */
        if (is_simulation_stopped(p->rules))
            break; /* Sortir si la simulation est terminée */
            
        /* PHASE 5: DORMIR */
        print_state(p->rules, p->id, "is sleeping"); /* Annoncer le sommeil */
        ms_sleep(p->rules->t_sleep); /* Dormir pendant le temps requis */
    }
    return (NULL); /* Fin du thread du philosophe */
}

int check_death(t_rules *r, t_philo *ph)
{
    int i = 0;
    /* Vérifier chaque philosophe pour voir s'il est mort */
    while (i < r->n_philo)
    {
        /* Protéger l'accès aux données de temps avec le mutex */
        pthread_mutex_lock(&r->death_mutex);
        long time_since_meal = timestamp_ms() - ph[i].last_meal_ms; /* Calculer le temps depuis le dernier repas */
        pthread_mutex_unlock(&r->death_mutex);
        
        /* Si le temps depuis le dernier repas dépasse le temps limite */
        if (time_since_meal > r->t_die)
        {
            print_state(r, ph[i].id, "died"); /* Afficher le message de mort */
            /* Marquer la simulation comme arrêtée de manière thread-safe */
            pthread_mutex_lock(&r->stop_mx);
            r->stop = 1; /* Arrêter la simulation */
            pthread_mutex_unlock(&r->stop_mx);
            return (1); /* Retourner 1 pour indiquer qu'un philosophe est mort */
        }
        i++; /* Passer au philosophe suivant */
    }
    return (0); /* Aucun philosophe n'est mort */
}

int check_meals_done(t_rules *r, t_philo *ph)
{
    int i = 0;
    int all_ate_enough = 1; /* Flag pour vérifier si tous ont assez mangé */
    
    /* Si aucun nombre de repas minimum n'est spécifié, ne pas vérifier */
    if (r->must_eat == -1)
        return (0); /* Pas de limite de repas, continuer la simulation */
        
    /* Vérifier le nombre de repas de chaque philosophe */
    while (i < r->n_philo)
    {
        /* Protéger l'accès au compteur de repas */
        pthread_mutex_lock(&r->death_mutex);
        int meals = ph[i].meals; /* Récupérer le nombre de repas du philosophe i */
        pthread_mutex_unlock(&r->death_mutex);
        
        /* Si ce philosophe n'a pas encore assez mangé */
        if (meals < r->must_eat)
        {
            all_ate_enough = 0; /* Pas tous ont assez mangé */
            break; /* Inutile de vérifier les autres */
        }
        i++; /* Passer au philosophe suivant */
    }
    
    /* Si tous les philosophes ont mangé le nombre requis de repas */
    if (all_ate_enough)
    {
        /* Arrêter la simulation de manière thread-safe */
        pthread_mutex_lock(&r->stop_mx);
        r->stop = 1; /* Marquer la simulation comme terminée */
        pthread_mutex_unlock(&r->stop_mx);
        return (1); /* Retourner 1 pour indiquer que tous ont assez mangé */
    }
    return (0); /* Certains philosophes n'ont pas encore assez mangé */
}

void *monitor_routine(void *arg)
{
    t_rules *r = (t_rules *)arg;
    
    /* Boucle de surveillance principale */
    while (!la_putain_de_simulation_qui_se_termine(r))
    {
        /* Vérifier si un philosophe est mort OU si tous ont assez mangé */
        if (regarde_si_le_fils_de_pute_est_pas_mort(r, r->philosophers) || nbr_de_repas(r, r->philosophers))
            break; /* Arrêter la surveillance si une condition d'arrêt est remplie */
        usleep(1000); /* Attendre 1ms avant la prochaine vérification */
    }
    return (NULL); /* Fin du thread monitor */
}

int regarde_si_le_fils_de_pute_est_pas_mort(t_rules *r, t_philo *ph)
{
    return check_death(r, ph);
}

int nbr_de_repas(t_rules *r, t_philo *ph)
{
    return check_meals_done(r, ph);
}

int la_putain_de_simulation_qui_se_termine(t_rules *r)
{
    return is_simulation_stopped(r);
}
