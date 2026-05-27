#include "codexion.h"

long long get_time_in_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

int ft_atoi(const char *str)
{
    long long res = 0;
    while (*str >= '0' && *str <= '9')
    {
        res = res * 10 + (*str - '0');
        if (res > 2147483647)
            return (-1);
        str++;
    }
    if (*str != '\0')
        return (-1);
    return ((int)res);
}

int ft_strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s1 == *s2)
    {
        s1++;
        s2++;
    }
    return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

void print_msg(t_coder *coder, const char *msg)
{
    long long time_now;

    pthread_mutex_lock(&coder->sim->print_lock);
    if (!check_stop(coder->sim))
    {
        time_now = get_time_in_ms() - coder->sim->start_time;
        printf("%lld %d %s\n", time_now, coder->id, msg);
    }
    pthread_mutex_unlock(&coder->sim->print_lock);
}

void ft_usleep(long long wait_time, t_sim *sim)
{
    long long start;

    start = get_time_in_ms();
    while ((get_time_in_ms() - start) < wait_time)
    {
        if (check_stop(sim))
            break;
        usleep(50);
    }
}
