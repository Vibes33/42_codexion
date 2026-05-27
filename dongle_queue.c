/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_queue.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ryan <ryan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 15:56:42 by ryan              #+#    #+#             */
/*   Updated: 2026/05/27 15:56:44 by ryan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	swap_request(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

static void	heapify_up(t_pqueue *q, int idx, t_policy policy)
{
	int	parent;

	while (idx > 0)
	{
		parent = (idx - 1) / 2;
		if (cmp_req(&q->requests[idx], &q->requests[parent], policy))
		{
			swap_request(&q->requests[idx], &q->requests[parent]);
			idx = parent;
		}
		else
			break ;
	}
}

static void	heapify_down(t_pqueue *q, int idx, t_policy policy)
{
	int	left;
	int	right;
	int	smallest;

	while (1)
	{
		left = 2 * idx + 1;
		right = 2 * idx + 2;
		smallest = idx;
		if (left < q->size && cmp_req(&q->requests[left],
				&q->requests[smallest], policy))
			smallest = left;
		if (right < q->size && cmp_req(&q->requests[right],
				&q->requests[smallest], policy))
			smallest = right;
		if (smallest != idx)
		{
			swap_request(&q->requests[idx], &q->requests[smallest]);
			idx = smallest;
		}
		else
			break ;
	}
}

void	enqueue(t_pqueue *q, t_request req, t_policy policy)
{
	if (q->size >= q->capacity)
		return ;
	q->requests[q->size] = req;
	heapify_up(q, q->size, policy);
	q->size++;
}

void	dequeue(t_pqueue *q, t_policy policy)
{
	if (q->size <= 0)
		return ;
	q->requests[0] = q->requests[q->size - 1];
	q->size--;
	if (q->size > 0)
		heapify_down(q, 0, policy);
}
