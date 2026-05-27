/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ryan <ryan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 15:57:18 by ryan              #+#    #+#             */
/*   Updated: 2026/05/27 15:57:19 by ryan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	perform_burnout_checks(t_sim *sim)
{
	int			i;
	long long	now;
	int			all_compiled;

	all_compiled = 1;
	i = 0;
	while (i < sim->nb_coders)
	{
		pthread_mutex_lock(&sim->coders[i].state_lock);
		now = get_time_in_ms();
		if (now - sim->coders[i].last_compile_start >= sim->t_burnout)
		{
			pthread_mutex_unlock(&sim->coders[i].state_lock);
			print_msg(&sim->coders[i], "burned out");
			set_stop(sim);
			return (1);
		}
		if (sim->nb_compiles_req == -1
			|| sim->coders[i].times_compiled < sim->nb_compiles_req)
			all_compiled = 0;
		pthread_mutex_unlock(&sim->coders[i].state_lock);
		i++;
	}
	return (all_compiled);
}

static void	*check_burnout(void *arg)
{
	t_sim	*sim;

	sim = (t_sim *)arg;
	while (!check_stop(sim))
	{
		if (perform_burnout_checks(sim))
		{
			if (sim->nb_compiles_req != -1)
				set_stop(sim);
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}

static int	init_coder_threads(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		if (pthread_create(&sim->coders[i].thread_id, NULL,
				&coder_routine, &sim->coders[i]) != 0)
			return (0);
		i++;
	}
	sim->start_time = get_time_in_ms();
	i = 0;
	while (i < sim->nb_coders)
	{
		pthread_mutex_lock(&sim->coders[i].state_lock);
		sim->coders[i].last_compile_start = sim->start_time;
		pthread_mutex_unlock(&sim->coders[i].state_lock);
		i++;
	}
	return (1);
}

int	start_simulation(t_sim *sim)
{
	int			i;
	pthread_t	monitor;

	if (!init_coder_threads(sim))
		return (0);
	if (pthread_create(&monitor, NULL, &check_burnout, sim) != 0)
		return (0);
	i = 0;
	while (i < sim->nb_coders)
	{
		pthread_join(sim->coders[i].thread_id, NULL);
		i++;
	}
	pthread_join(monitor, NULL);
	return (1);
}
