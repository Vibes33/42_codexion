/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ryan <ryan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 15:56:31 by ryan              #+#    #+#             */
/*   Updated: 2026/05/27 15:56:35 by ryan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	coder_cycle(t_coder *coder)
{
	take_dongles(coder);
	if (check_stop(coder->sim))
		return (0);
	pthread_mutex_lock(&coder->state_lock);
	coder->last_compile_start = get_time_in_ms();
	pthread_mutex_unlock(&coder->state_lock);
	print_msg(coder, "is compiling");
	ft_usleep(coder->sim->t_compile, coder->sim);
	pthread_mutex_lock(&coder->state_lock);
	coder->times_compiled++;
	pthread_mutex_unlock(&coder->state_lock);
	release_dongles(coder);
	if (check_stop(coder->sim))
		return (0);
	print_msg(coder, "is debugging");
	ft_usleep(coder->sim->t_debug, coder->sim);
	if (check_stop(coder->sim))
		return (0);
	print_msg(coder, "is refactoring");
	ft_usleep(coder->sim->t_refactor, coder->sim);
	return (1);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (coder->sim->start_time == 0)
		usleep(100);
	if (coder->id % 2 == 0)
		ft_usleep(coder->sim->t_compile / 2, coder->sim);
	while (!check_stop(coder->sim))
	{
		if (!coder_cycle(coder))
			break ;
	}
	return (NULL);
}
