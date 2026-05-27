/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ryan <ryan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 15:57:03 by ryan              #+#    #+#             */
/*   Updated: 2026/05/27 15:57:03 by ryan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	t_sim	sim;

	memset(&sim, 0, sizeof(t_sim));
	if (argc != 9)
	{
		printf("Error: invalid number of arguments\n");
		return (1);
	}
	if (parse_args(&sim, argv) == 0)
	{
		printf("Error: invalid arguments\n");
		return (1);
	}
	if (init_simulation(&sim) == 0)
	{
		printf("Error: init failed\n");
		return (1);
	}
	start_simulation(&sim);
	clean_simulation(&sim);
	return (0);
}
