#include "codexion.h"

/* Swap function for priority queue */
static void swap_request(t_request *a, t_request *b)
{
    t_request tmp = *a;
    *a = *b;
    *b = tmp;
}

/* Heapify Up */
static void heapify_up(t_pqueue *q, int idx, t_policy policy)
{
    int parent;

    while (idx > 0)
    {
        parent = (idx - 1) / 2;
        int cmp = 0;
        if (policy == EDF)
        {
            if (q->requests[idx].deadline < q->requests[parent].deadline)
                cmp = 1;
            else if (q->requests[idx].deadline == q->requests[parent].deadline)
            {
                if (q->requests[idx].arrival_time < q->requests[parent].arrival_time)
                    cmp = 1;
            }
        }
        else // FIFO
        {
            if (q->requests[idx].arrival_time < q->requests[parent].arrival_time)
                cmp = 1;
        }

        if (cmp)
        {
            swap_request(&q->requests[idx], &q->requests[parent]);
            idx = parent;
        }
        else
            break;
    }
}

/* Heapify Down */
static void heapify_down(t_pqueue *q, int idx, t_policy policy)
{
    int left, right, smallest;

    while (1)
    {
        left = 2 * idx + 1;
        right = 2 * idx + 2;
        smallest = idx;

        if (left < q->size)
        {
            int cmp_l = 0;
            if (policy == EDF)
            {
                if (q->requests[left].deadline < q->requests[smallest].deadline)
                    cmp_l = 1;
                else if (q->requests[left].deadline == q->requests[smallest].deadline)
                {
                    if (q->requests[left].arrival_time < q->requests[smallest].arrival_time)
                        cmp_l = 1;
                }
            }
            else
            {
                if (q->requests[left].arrival_time < q->requests[smallest].arrival_time)
                    cmp_l = 1;
            }
            if (cmp_l)
                smallest = left;
        }

        if (right < q->size)
        {
            int cmp_r = 0;
            if (policy == EDF)
            {
                if (q->requests[right].deadline < q->requests[smallest].deadline)
                    cmp_r = 1;
                else if (q->requests[right].deadline == q->requests[smallest].deadline)
                {
                    if (q->requests[right].arrival_time < q->requests[smallest].arrival_time)
                        cmp_r = 1;
                }
            }
            else
            {
                if (q->requests[right].arrival_time < q->requests[smallest].arrival_time)
                    cmp_r = 1;
            }
            if (cmp_r)
                smallest = right;
        }

        if (smallest != idx)
        {
            swap_request(&q->requests[idx], &q->requests[smallest]);
            idx = smallest;
        }
        else
            break;
    }
}

static void enqueue(t_pqueue *q, t_request req, t_policy policy)
{
    if (q->size >= q->capacity)
        return;
    q->requests[q->size] = req;
    heapify_up(q, q->size, policy);
    q->size++;
}

static void dequeue(t_pqueue *q, t_policy policy)
{
    if (q->size <= 0)
        return;
    q->requests[0] = q->requests[q->size - 1];
    q->size--;
    if (q->size > 0)
        heapify_down(q, 0, policy);
}

static void wait_for_dongle(t_coder *coder, t_dongle *d)
{
    t_request req;

    pthread_mutex_lock(&d->lock);
    
    pthread_mutex_lock(&coder->state_lock);
    req.coder_id = coder->id;
    req.deadline = coder->last_compile_start + coder->sim->t_burnout;
    pthread_mutex_unlock(&coder->state_lock);
    req.arrival_time = get_time_in_ms();
    
    enqueue(&d->waitlist, req, coder->sim->scheduler);

    while (1)
    {
        if (check_stop(coder->sim))
            break;
        
        long long current_time = get_time_in_ms();
        if (d->is_available && (d->available_at <= current_time) 
            && d->waitlist.size > 0 && d->waitlist.requests[0].coder_id == coder->id)
        {
            d->is_available = 0;
            dequeue(&d->waitlist, coder->sim->scheduler);
            break;
        }

        // Si cooldown non passé on peut utiliser timedwait
        if (d->available_at > current_time)
        {
            struct timespec ts;
            long long wake = d->available_at;
            ts.tv_sec = wake / 1000;
            ts.tv_nsec = (wake % 1000) * 1000000;
            pthread_cond_timedwait(&d->cv, &d->lock, &ts);
        }
        else
        {
            pthread_cond_wait(&d->cv, &d->lock);
        }
    }
    
    pthread_mutex_unlock(&d->lock);
}

void take_dongles(t_coder *coder)
{
    // Fix deadlocks using a canonical lock order
    t_dongle *first_d = coder->left_dongle;
    t_dongle *second_d = coder->right_dongle;
    
    if (first_d->id > second_d->id)
    {
        first_d = coder->right_dongle;
        second_d = coder->left_dongle;
    }
    
    wait_for_dongle(coder, first_d);
    print_msg(coder, "has taken a dongle");
    
    if (coder->sim->nb_coders == 1) // Edge case 1 coder
    {
        ft_usleep(coder->sim->t_burnout + 10, coder->sim); 
        return;
    }
    
    wait_for_dongle(coder, second_d);
    print_msg(coder, "has taken a dongle");
}

void release_dongles(t_coder *coder)
{
    long long avail = get_time_in_ms() + coder->sim->cooldown;
    
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
