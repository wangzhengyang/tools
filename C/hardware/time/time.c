#include "time.h"
#include <time.h>
#include <sys/time.h>

uint64_t hal_get_mstime()
{
    struct timeval tp;

    gettimeofday(&tp, NULL);

    return ((uint64_t)tp.tv_sec * 1000LL + (tp.tv_usec / 1000));
}
