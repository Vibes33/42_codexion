/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ryan <ryan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 00:00:00 by ryan              #+#    #+#             */
/*   Updated: 2024/05/27 00:00:00 by ryan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

typedef enum e_policy
{
	FIFO,
	EDF
}	t_policy;

typedef struct s_sim	t_sim;

typedef struct s_request
{
	int			coder_id;
	long long	deadline;
	long long	arrival_time;
}	t_request;

typedef struct s_pqueue
{
	t_request	*requests;
	int			size;
	int			capacity;
}	t_pqueue;

typedef struct s_dongle
{
	int				id;
	pthread_mutex_t	lock;
	pthread_cond_t	cv;
	int				is_available;
	long long		available_at;
	t_pqueue		waitlist;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	pthread_t		thread_id;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	pthread_mutex_t	state_lock;
	long long		last_compile_start;
	int				times_compiled;
	t_sim			*sim;
}	t_coder;

struct s_sim
{
	int				nb_coders;
	long long		t_burnout;
	long long		t_compile;
	long long		t_debug;
	long long		t_refactor;
	int				nb_compiles_req;
	long long		cooldown;
	t_policy		scheduler;
	t_dongle		*dongles;
	t_coder			*coders;
	long long		start_time;
	pthread_mutex_t	print_lock;
	pthread_mutex_t	sim_lock;
	int				stop_flag;
};

/* utils.c */
long long	get_time_in_ms(void);
int			ft_atoi(const char *str);
int			ft_strcmp(const char *s1, const char *s2);
void		print_msg(t_coder *coder, const char *msg);
void		ft_usleep(long long wait_time, t_sim *sim);

/* parsing.c */
int			parse_args(t_sim *sim, char **argv);

/* init.c */
int			init_simulation(t_sim *sim);
void		clean_simulation(t_sim *sim);

/* sim.c */
int			start_simulation(t_sim *sim);
int			check_stop(t_sim *sim);
void		set_stop(t_sim *sim);

/* coder.c */
void		*coder_routine(void *arg);

/* dongle.c */
void		take_dongles(t_coder *coder);
void		release_dongles(t_coder *coder);

/* dongle_queue.c */
void		enqueue(t_pqueue *q, t_request req, t_policy policy);
void		dequeue(t_pqueue *q, t_policy policy);
int			cmp_req(t_request *a, t_request *b, t_policy policy);
#endif
