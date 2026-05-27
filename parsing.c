#include "codexion.h"

int parse_args(t_sim *sim, char **argv)
{
    sim->nb_coders = ft_atoi(argv[1]);
    sim->t_burnout = ft_atoi(argv[2]);
    sim->t_compile = ft_atoi(argv[3]);
    sim->t_debug = ft_atoi(argv[4]);
    sim->t_refactor = ft_atoi(argv[5]);
    sim->nb_compiles_req = ft_atoi(argv[6]);
    sim->cooldown = ft_atoi(argv[7]);

    if (sim->nb_coders <= 0 || sim->t_burnout < 0 || sim->t_compile < 0 
        || sim->t_debug < 0 || sim->t_refactor < 0 
        || sim->nb_compiles_req < 0 || sim->cooldown < 0)
        return (0);

    if (ft_strcmp(argv[8], "fifo") == 0)
        sim->scheduler = FIFO;
    else if (ft_strcmp(argv[8], "edf") == 0)
        sim->scheduler = EDF;
    else
        return (0);

    return (1);
}
