#include "codexion.h"

void *coder_routine(void *arg)
{
    t_coder *coder = (t_coder *)arg;

    // sync start
    while (coder->sim->start_time == 0)
        usleep(100);

    // Initial delay for some coders to avoid all rushing at the exact same moment
    if (coder->id % 2 == 0)
        ft_usleep(coder->sim->t_compile / 2, coder->sim);

    while (!check_stop(coder->sim))
    {
        take_dongles(coder);
        if (check_stop(coder->sim))
            break;

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
            break;

        print_msg(coder, "is debugging");
        ft_usleep(coder->sim->t_debug, coder->sim);
        if (check_stop(coder->sim))
            break;

        print_msg(coder, "is refactoring");
        ft_usleep(coder->sim->t_refactor, coder->sim);
    }
    return (NULL);
}
