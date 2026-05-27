#include "codexion.h"

static int init_dongles(t_sim *sim)
{
    int i;

    i = 0;
    while (i < sim->nb_coders)
    {
        sim->dongles[i].id = i;
        sim->dongles[i].is_available = 1;
        sim->dongles[i].available_at = 0;
        
        sim->dongles[i].waitlist.size = 0;
        sim->dongles[i].waitlist.capacity = sim->nb_coders;
        sim->dongles[i].waitlist.requests = malloc(sizeof(t_request) * sim->nb_coders);
        if (!sim->dongles[i].waitlist.requests)
            return (0);

        if (pthread_mutex_init(&sim->dongles[i].lock, NULL) != 0)
            return (0);
        if (pthread_cond_init(&sim->dongles[i].cv, NULL) != 0)
            return (0);
        i++;
    }
    return (1);
}

static int init_coders(t_sim *sim)
{
    int i;

    i = 0;
    while (i < sim->nb_coders)
    {
        sim->coders[i].id = i + 1;
        sim->coders[i].sim = sim;
        sim->coders[i].times_compiled = 0;
        sim->coders[i].last_compile_start = 0;
        
        sim->coders[i].left_dongle = &sim->dongles[i];
        sim->coders[i].right_dongle = &sim->dongles[(i + 1) % sim->nb_coders];
        
        if (pthread_mutex_init(&sim->coders[i].state_lock, NULL) != 0)
            return (0);
        i++;
    }
    return (1);
}

int init_simulation(t_sim *sim)
{
    sim->stop_flag = 0;
    
    if (pthread_mutex_init(&sim->print_lock, NULL) != 0)
        return (0);
    if (pthread_mutex_init(&sim->sim_lock, NULL) != 0)
        return (0);

    sim->dongles = malloc(sizeof(t_dongle) * sim->nb_coders);
    if (!sim->dongles)
        return (0);

    sim->coders = malloc(sizeof(t_coder) * sim->nb_coders);
    if (!sim->coders)
    {
        free(sim->dongles);
        return (0);
    }

    if (!init_dongles(sim) || !init_coders(sim))
        return (0);

    return (1);
}

void clean_simulation(t_sim *sim)
{
    int i;

    i = 0;
    while (i < sim->nb_coders)
    {
        if (sim->dongles[i].waitlist.requests)
            free(sim->dongles[i].waitlist.requests);
        pthread_mutex_destroy(&sim->dongles[i].lock);
        pthread_cond_destroy(&sim->dongles[i].cv);
        pthread_mutex_destroy(&sim->coders[i].state_lock);
        i++;
    }
    pthread_mutex_destroy(&sim->print_lock);
    pthread_mutex_destroy(&sim->sim_lock);
    if (sim->dongles)
        free(sim->dongles);
    if (sim->coders)
        free(sim->coders);
}
