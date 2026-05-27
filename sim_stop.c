/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_stop.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ryan <ryan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 15:57:12 by ryan              #+#    #+#             */
/*   Updated: 2026/05/27 15:57:13 by ryan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	check_stop(t_sim *sim)
{
	int	flag;

	pthread_mutex_lock(&sim->sim_lock);
	flag = sim->stop_flag;
	pthread_mutex_unlock(&sim->sim_lock);
	return (flag);
}

void	set_stop(t_sim *sim)
{
	int	i;

	pthread_mutex_lock(&sim->sim_lock);
	sim->stop_flag = 1;
	pthread_mutex_unlock(&sim->sim_lock);
	i = 0;
	while (i < sim->nb_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].lock);
		pthread_cond_broadcast(&sim->dongles[i].cv);
		pthread_mutex_unlock(&sim->dongles[i].lock);
		i++;
	}
}
