*This project has been created as part of the 42 curriculum by ryan.*

# Codexion

## Description
Codexion is a concurrency simulation where multiple coders share a limited number of USB dongles to compile their quantum code. It highlights modern thread synchronization techniques, deadlock prevention, and scheduling principles avoiding burnout through fair resource distribution.

## Instructions
To build the project, run:
```bash
make
```

To run the simulation:
```bash
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <scheduler>
```
Example:
```bash
./codexion 4 800 200 200 200 7 10 edf
```

## Resources
- Operating Systems: Three Easy Pieces (Concurrency chapters)
- POSIX Threads Programming documentation (LLNL)
- Information gathered on EDF and FIFO priority queues.
- No AI generated code was blindly pasted; AI served as an assistant to review Norminette compliance and debug parsing functions.

## Blocking cases handled
- **Deadlock prevention:** We prevent circular wait (one of Coffman's conditions) by enforcing a strict lock ordering mechanism. Whenever a coder tries to pick up dongles, they will lock the dongle with the lowest ID first, regardless of whether it is initially on their left or right side.
- **Starvation prevention:** When multiple coders request the same dongle, it applies either strict Priority Queue (EDF/FIFO) combined with Condition Variables. Starvation is thus mitigated as the most vulnerable coder (under EDF) or the longest waiting (under FIFO) gets served gracefully.
- **Cooldown handling:** The cooldown guarantees fairness and mimics hardware limitations by applying `pthread_cond_timedwait` based on real-time availability timestamps.
- **Precise burnout detection:** The simulation deploys a detached monitor thread iterating rapidly to check if the timestamp between the last compilation and current time exceeds the allowed burnout ceiling.
- **Log serialization:** The output is controlled by a dedicated `pthread_mutex_t print_lock` avoiding garbled interleaved messages.

## Thread synchronization mechanisms
- `pthread_mutex_t`: Used to protect the state variables of the coders, dongles, simulation flags, and properly isolate standard output.
- `pthread_cond_t`: Acts as a signaling tool on dongles; when a dongle is released, the associated condition variable broadcasts or signals to the waiting queue (via `pthread_cond_broadcast()`) that the lock might be accessible and conditions are evaluated again.
- Timing dependencies inside waiting mechanisms use `pthread_cond_timedwait()`, unlocking the mutex gracefully over the required cooldown without busy-waiting.
