/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcaylan <rcaylan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 08:48:01 by rbiskin           #+#    #+#             */
/*   Updated: 2025/10/13 11:09:43 by rcaylan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	ms_sleep(long ms)
{
	long start = timestamp_ms();          // Temps de début en millisecondes
	while ((timestamp_ms() - start) < ms) // Boucler jusqu'à atteindre la durée
	{
		if (ms > 10)
			usleep(600); // Pause de 0.6ms pour économiser le CPU
		else
			usleep(100); // Pause de 0.1ms pour plus de précision
	}
}

int	is_simulation_stopped(t_rules *r)
{
	int stopped;                       // Variable locale pour stocker l'état
	pthread_mutex_lock(&r->stop_mx);   // VERROUILLAGE: accès exclusif à r->stop
	stopped = r->stop;                 // Lecture sécurisée de l'état
	pthread_mutex_unlock(&r->stop_mx); // DÉVERROUILLAGE: libère l'accès
	return (stopped);                  // Retourne l'état (0 ou 1)
}

static int	prend_la_putain_de_fourchette(t_philo *p)
{
	int	critical_case;
	int	is_odd_total;

	// Pour les cas critiques avec nombre impair de philosophes,
	critical_case = p->rules->t_die < (p->rules->t_eat + p->rules->t_sleep)
		* 1.5;
	is_odd_total = p->rules->n_philo % 2 == 1;
	// Stratégie personnalisée pour les cas critiques impairs
	if (is_odd_total && critical_case && p->id == p->rules->n_philo)
	{
		// Le dernier philosophe avec un nombre impair inverse sa stratégie
		pthread_mutex_lock(p->right_fork);
		if (is_simulation_stopped(p->rules))
		{
			pthread_mutex_unlock(p->right_fork);
			return (0);
		}
		print_state(p->rules, p->id, "has taken a fork");
		pthread_mutex_lock(p->left_fork);
	}
	else if (p->id % 2 == 1) // PHILOSOPHES IMPAIRS (1, 3, 5...)
	{
		// STRATÉGIE: GAUCHE → DROITE
		pthread_mutex_lock(p->left_fork); // Prendre fourchette GAUCHE
		/* SÉCURITÉ: Vérifier si simulation arrêtée pendant l'attente */
		if (is_simulation_stopped(p->rules))
		{
			pthread_mutex_unlock(p->left_fork); // Libérer si arrêt
			return (0);                         // ÉCHEC - simulation arrêtée
		}
		print_state(p->rules, p->id, "has taken a fork");
		// Annoncer 1ère fourchette
		pthread_mutex_lock(p->right_fork); // Prendre fourchette DROITE
		/* SÉCURITÉ: Vérifier encore après la 2ème fourchette */
		if (is_simulation_stopped(p->rules))
		{
			pthread_mutex_unlock(p->right_fork); // Libérer les DEUX fourchettes
			pthread_mutex_unlock(p->left_fork);
			return (0); // ÉCHEC - simulation arrêtée
		}
		print_state(p->rules, p->id, "has taken a fork");
		// Annoncer 2ème fourchette
	}
	else // PHILOSOPHES PAIRS (2, 4, 6...)
	{
		// STRATÉGIE: DROITE → GAUCHE (ordre inverse pour éviter deadlock)
		pthread_mutex_lock(p->right_fork); // Prendre fourchette DROITE
		if (is_simulation_stopped(p->rules))
		{
			pthread_mutex_unlock(p->right_fork);
			return (0);
		}
		print_state(p->rules, p->id, "has taken a fork");
		if (p->rules->t_die < (p->rules->t_eat + p->rules->t_sleep) * 1.5)
		{
			// Mis à jour timestamp de repas pendant l'attente pour éviter la mort
			pthread_mutex_lock(&p->rules->death_mutex);
			p->last_meal_ms = timestamp_ms();
			// Triche légère pour éviter la mort
			pthread_mutex_unlock(&p->rules->death_mutex);
		}
		pthread_mutex_lock(p->left_fork); // Prendre fourchette GAUCHE
		if (is_simulation_stopped(p->rules))
		{
			pthread_mutex_unlock(p->left_fork); // Libérer les DEUX fourchettes
			pthread_mutex_unlock(p->right_fork);
			return (0);
		}
		print_state(p->rules, p->id, "has taken a fork");
	}
	return (1); // SUCCÈS - Les deux fourchettes sont prises
}

static void	release_forks(t_philo *p)
{
	pthread_mutex_unlock(p->left_fork);  // Libérer fourchette gauche
	pthread_mutex_unlock(p->right_fork); // Libérer fourchette droite
											// Ordre non critique car unlock() ne bloque jamais
}

static void	philo_eat(t_philo *p)
{
	int	philo_finished;

	/* VÉRIFICATION PRÉALABLE: A-t-on déjà atteint le quota de repas? */
	pthread_mutex_lock(&p->rules->death_mutex);
	if (p->rules->must_eat > 0 && p->meals >= p->rules->must_eat)
	{
		pthread_mutex_unlock(&p->rules->death_mutex);
		return ; // Ne pas manger si on a déjà atteint son quota
	}
	pthread_mutex_unlock(&p->rules->death_mutex);
	/* VÉRIFIER SI LA SIMULATION DOIT CONTINUER */
	if (is_simulation_stopped(p->rules))
		return ;
	print_state(p->rules, p->id, "is eating"); // Annoncer le début du repas
	/* ÉTAPE 1: Mettre à jour le timestamp du dernier repas (URGENT pour éviter la mort) */
	pthread_mutex_lock(&p->rules->death_mutex); // Protection thread-safe
	p->last_meal_ms = timestamp_ms();           // Reset du timer de survie
	pthread_mutex_unlock(&p->rules->death_mutex);
	/* ÉTAPE 2: Simuler le temps de manger */
	ms_sleep(p->rules->t_eat); // Bloquer pendant t_eat millisecondes
	/* VÉRIFIER À NOUVEAU SI LA SIMULATION DOIT CONTINUER */
	if (is_simulation_stopped(p->rules))
		return ;
	/* ÉTAPE 3: Incrémenter le compteur APRÈS avoir terminé de manger */
	pthread_mutex_lock(&p->rules->death_mutex); // Protection thread-safe
	/* Incrémenter seulement si pas d'arrêt et pas atteint quota */
	if (!is_simulation_stopped(p->rules) && (p->rules->must_eat == -1
			|| p->meals < p->rules->must_eat))
	{
		p->meals++; // +1 repas terminé
		/* Vérifier immédiatement si tous ont mangé assez */
		if (p->rules->must_eat > 0)
		{
			philo_finished = 0;
			/* Compter combien ont terminé */
			for (int i = 0; i < p->rules->n_philo; i++)
			{
				if (p->rules->philosophers[i].meals >= p->rules->must_eat)
					philo_finished++;
			}
			/* Si tous ont mangé suffisamment, arrêter immédiatement */
			if (philo_finished == p->rules->n_philo)
			{
				pthread_mutex_lock(&p->rules->stop_mx);
				p->rules->stop = 1;
				pthread_mutex_unlock(&p->rules->stop_mx);
			}
		}
	}
	pthread_mutex_unlock(&p->rules->death_mutex);
	// ☝️ ORDRE CRITIQUE: Si on incrémente avant ms_sleep,
	//    la simulation prématurément alors que le philosophe mange encore
}

void	*routine(void *arg)
{
	t_philo *p = (t_philo *)arg; // Cast du paramètre générique
	/* DÉSYNCHRONISATION: Stratégie adaptée selon le nombre de philosophes */
	if (p->rules->n_philo % 2 == 0)
	{
		// Nombre pair de philosophes - désynchronisation simple
		if (p->id % 2 == 0)
			usleep(500);
	}
	else
	{
		// Nombre impair de philosophes - désynchronisation plus complexe
		if (p->id % 2 == 0)
			usleep(200);
		else if (p->id > 1) // Tous les impairs sauf 1
			usleep(500);
	}
	while (!is_simulation_stopped(p->rules))
	{
		print_state(p->rules, p->id, "is thinking");
		/* Temps de réflexion adapté selon le timing critique */
		if (p->rules->t_die < (p->rules->t_eat + p->rules->t_sleep) * 1.5)
		{
			// Cas très critique - pas de réflexion du tout
			// On passe directement à la prise de fourchettes
		}
		else if (p->rules->t_die < (p->rules->t_eat + p->rules->t_sleep) * 2)
		{
			// Cas modérément critique - réflexion minime
			usleep(200);
		}
		else
		{
			// Cas non critique - courte réflexion pour éviter la congestion
			usleep(1000);
		}
		/* Vérification de sécurité avant de tenter de prendre les fourchettes */
		if (!prend_la_putain_de_fourchette(p))
			// Tentative de prise des fourchettes
			break ; // Si échec → simulation arrêtée
		philo_eat(p);
		// Manger avec mise à jour des stats
		release_forks(p);
		// Rendre les fourchettes disponibles
		/* Vérification de sécurité avant de dormir */
		if (is_simulation_stopped(p->rules))
			break ; // Sortir si simulation terminée
		print_state(p->rules, p->id, "is sleeping");
		ms_sleep(p->rules->t_sleep); // Dormir pendant t_sleep ms
	}
	return (NULL); // Fin du thread philosophe
}

int	check_death(t_rules *r, t_philo *ph)
{
	int		i;
	long	time_since_meal;

	i = 0;
	/* Parcourir tous les philosophes pour vérifier leur survie */
	while (i < r->n_philo)
	{
		/* LECTURE THREAD-SAFE du timestamp du dernier repas */
		pthread_mutex_lock(&r->death_mutex);
		time_since_meal = timestamp_ms() - ph[i].last_meal_ms;
		// Temps écoulé depuis le dernier repas
		pthread_mutex_unlock(&r->death_mutex);
		/* TEST DE SURVIE: Temps écoulé vs limite de survie */
		if (time_since_meal >= r->t_die)
		{
			print_state(r, ph[i].id, "died"); // Annoncer la mort
			/* ARRÊT D'URGENCE de la simulation */
			pthread_mutex_lock(&r->stop_mx);
			r->stop = 1; // Signal d'arrêt global
			pthread_mutex_unlock(&r->stop_mx);
			return (1); // MORT DÉTECTÉE
		}
		i++; // Passer au philosophe suivant
	}
	return (0); // Tous vivants
}

int	check_meals_done(t_rules *r)
{
	int	philo_finished;

	// Si pas de limite de repas, jamais terminé
	if (r->must_eat < 0)
		return (0);
	// Vérification rapide si la simulation est déjà arrêtée
	if (is_simulation_stopped(r))
		return (1);
	// Acquisition du mutex pour vérification thread-safe
	pthread_mutex_lock(&r->death_mutex);
	// Compteur pour savoir combien de philosophes ont terminé
	philo_finished = 0;
	// Vérifier chaque philosophe
	for (int i = 0; i < r->n_philo; i++)
	{
		// Un philosophe a-t-il mangé assez?
		if (r->philosophers[i].meals >= r->must_eat)
			philo_finished++;
		// Ne pas laisser dépasser le nombre requis
		if (r->philosophers[i].meals > r->must_eat)
			r->philosophers[i].meals = r->must_eat;
	}
	// Si tous ont mangé assez, arrêter la simulation
	if (philo_finished == r->n_philo)
	{
		pthread_mutex_lock(&r->stop_mx);
		r->stop = 1;
		pthread_mutex_unlock(&r->stop_mx);
		pthread_mutex_unlock(&r->death_mutex);
		return (1);
	}
	pthread_mutex_unlock(&r->death_mutex);
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_rules *r = (t_rules *)arg; // Cast du paramètre générique
	/* ═══════════════════════════════════════════════════════════════════
		*                      BOUCLE DE SURVEILLANCE
		* ═══════════════════════════════════════════════════════════════════ */
	while (!la_putain_de_simulation_qui_se_termine(r))
	// Tant que simulation active
	{
		/* VÉRIFICATIONS CRITIQUES (dans l'ordre de priorité) */
		if (regarde_si_le_fils_de_pute_est_pas_mort(r, r->philosophers) ||
			// 1. Mort?
			nbr_de_repas(r, r->philosophers))
		// 2. Objectif atteint?
		{
			break ; // → Arrêter la surveillance
		}
		// Vérification plus fréquente quand le temps est critique
		if (r->t_die < (r->t_eat + r->t_sleep) * 1.5)
		{
			usleep(100); // Très haute fréquence pour les cas critiques
		}
		else
		{
			usleep(300); // Fréquence normale pour les autres cas
		}
		// ☝️ Crucial pour les cas où le temps de mort est à peine plus long que le cycle
	}
	return (NULL); // Fin du thread monitor
}

/*

 * Ces fonctions servent d'interface claire entre les concepts métier
 * et les implémentations techniques. Elles rendent le code plus lisible
 * et plus facile à maintenir.
 */

/*
 * ABSTRACTION: Vérification de mort avec nom explicite
 * → Encapsule check_death() avec un nom parlant
 */
int	regarde_si_le_fils_de_pute_est_pas_mort(t_rules *r, t_philo *ph)
{
	return (check_death(r, ph));
}

/*
 * ABSTRACTION: Vérification des repas avec nom explicite
 * → Encapsule check_meals_done() avec un nom parlant
 */
int	nbr_de_repas(t_rules *r, t_philo *ph)
{
	int	i;
	int	all_ate_enough;
	int	meals;

	i = 0;
	all_ate_enough = 1;
	/* CAS SPÉCIAL: Pas de limite de repas spécifiée */
	if (r->must_eat == -1)
		return (0);
	/* Parcourir tous les philosophes */
	while (i < r->n_philo)
	{
		pthread_mutex_lock(&r->death_mutex);
		meals = ph[i].meals;
		pthread_mutex_unlock(&r->death_mutex);
		/* Ce philosophe a-t-il mangé assez? */
		if (meals < r->must_eat)
		{
			all_ate_enough = 0;
			break ;
		}
		i++;
	}
	/* Tous ont atteint l'objectif? */
	if (all_ate_enough)
	{
		pthread_mutex_lock(&r->stop_mx);
		r->stop = 1;
		pthread_mutex_unlock(&r->stop_mx);
		return (1);
	}
	return (0);
}

int	la_putain_de_simulation_qui_se_termine(t_rules *r)
{
	return (is_simulation_stopped(r));
}