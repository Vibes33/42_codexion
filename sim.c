#include "codexion.h"

int check_stop(t_sim *sim)
{
    int flag;

    pthread_mutex_lock(&sim->sim_lock);
    flag = sim->stop_flag;
    pthread_mutex_unlock(&sim->sim_lock);
    return (flag);
}

void set_stop(t_sim *sim)
{
    pthread_mutex_lock(&sim->sim_lock);
    sim->stop_flag = 1;
    pthread_mutex_unlock(&sim->sim_lock);
    
    // Broadcast all to release waiters
    for (int i = 0; i < sim->nb_coders; i++)
    {
        pthread_mutex_lock(&sim->dongles[i].lock);
        pthread_cond_broadcast(&sim->dongles[i].cv);
        pthread_mutex_unlock(&sim->dongles[i].lock);
    }
}

static void *check_burnout(void *arg)
{
    t_sim *sim = (t_sim *)arg;
    int i;
    long long now;
    int all_compiled;

    while (!check_stop(sim))
    {
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
                return (NULL);
            }
            if (sim->nb_compiles_req == -1 || sim->coders[i].times_compiled < sim->nb_compiles_req)
                all_compiled = 0;
            pthread_mutex_unlock(&sim->coders[i].state_lock);
            i++;
        }
        if (sim->nb_compiles_req != -1 && all_compiled)
        {
            set_stop(sim);
            return (NULL);
        }
        usleep(1000); // 1ms
    }
    return (NULL);
}

int start_simulation(t_sim *sim)
{
    int i;
    pthread_t monitor;

    i = 0;
    while (i < sim->nb_coders)
    {
        if (pthread_create(&sim->coders[i].thread_id, NULL, &coder_routine, &sim->coders[i]) != 0)
            return (0);
        i++;
    }

    sim->start_time = get_time_in_ms();
    for (i = 0; i < sim->nb_coders; i++)
    {
        pthread_mutex_lock(&sim->coders[i].state_lock);
        sim->coders[i].last_compile_start = sim->start_time;
        pthread_mutex_unlock(&sim->coders[i].state_lock);
    }

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
