/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ryan <ryan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 15:56:48 by ryan              #+#    #+#             */
/*   Updated: 2026/05/27 15:56:49 by ryan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	cmp_req(t_request *a, t_request *b, t_policy policy)
{
	if (policy == EDF)
	{
		if (a->deadline < b->deadline)
			return (1);
		if (a->deadline == b->deadline)
		{
			if (a->arrival_time < b->arrival_time)
				return (1);
		}
		return (0);
	}
	if (a->arrival_time < b->arrival_time)
		return (1);
	return (0);
}

static void	wait_dongle_loop(t_coder *c, t_dongle *d)
{
	long long		curr;
	struct timespec	ts;

	while (!check_stop(c->sim))
	{
		curr = get_time_in_ms();
		if (d->is_available && d->available_at <= curr
			&& d->waitlist.size > 0
			&& d->waitlist.requests[0].coder_id == c->id)
		{
			d->is_available = 0;
			dequeue(&d->waitlist, c->sim->scheduler);
			break ;
		}
		if (d->available_at > curr)
		{
			ts.tv_sec = d->available_at / 1000;
			ts.tv_nsec = (d->available_at % 1000) * 1000000;
			pthread_cond_timedwait(&d->cv, &d->lock, &ts);
		}
		else
			pthread_cond_wait(&d->cv, &d->lock);
	}
}

static void	wait_for_dongle(t_coder *coder, t_dongle *d)
{
	t_request	req;

	pthread_mutex_lock(&d->lock);
	pthread_mutex_lock(&coder->state_lock);
	req.coder_id = coder->id;
	req.deadline = coder->last_compile_start + coder->sim->t_burnout;
	pthread_mutex_unlock(&coder->state_lock);
	req.arrival_time = get_time_in_ms();
	enqueue(&d->waitlist, req, coder->sim->scheduler);
	wait_dongle_loop(coder, d);
	pthread_mutex_unlock(&d->lock);
}

void	take_dongles(t_coder *coder)
{
	t_dongle	*first_d;
	t_dongle	*second_d;

	first_d = coder->left_dongle;
	second_d = coder->right_dongle;
	if (first_d->id > second_d->id)
	{
		first_d = coder->right_dongle;
		second_d = coder->left_dongle;
	}
	wait_for_dongle(coder, first_d);
	print_msg(coder, "has taken a dongle");
	if (coder->sim->nb_coders == 1)
	{
		ft_usleep(coder->sim->t_burnout + 10, coder->sim);
		return ;
	}
	wait_for_dongle(coder, second_d);
	print_msg(coder, "has taken a dongle");
}

void	release_dongles(t_coder *coder)
{
	long long	avail;

	avail = get_time_in_ms() + coder->sim->cooldown;
	pthread_mutex_lock(&coder->left_dongle->lock);
	coder->left_dongle->is_available = 1;
	coder->left_dongle->available_at = avail;
	pthread_cond_broadcast(&coder->left_dongle->cv);
	pthread_mutex_unlock(&coder->left_dongle->lock);
	if (coder->sim->nb_coders > 1)
	{
		pthread_mutex_lock(&coder->right_dongle->lock);
		coder->right_dongle->is_available = 1;
		coder->right_dongle->available_at = avail;
		pthread_cond_broadcast(&coder->right_dongle->cv);
		pthread_mutex_unlock(&coder->right_dongle->lock);
	}
}
