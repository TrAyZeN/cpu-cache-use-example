#define _GNU_SOURCE
#include <assert.h>
#include <err.h>
#include <sched.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "utils.h"

// Pin current thread to cpu
void pin_to_cpu(size_t cpu) {
    size_t num_cpus = (size_t)sysconf(_SC_NPROCESSORS_ONLN);
    if (num_cpus == (size_t)-1L) {
        err(1, "Failed to get number of processors");
    }

    // Create a cpu_set_t with a single cpu
    cpu_set_t *cpu_set = CPU_ALLOC(num_cpus);
    if (cpu_set == NULL) {
        err(1, "Failed to allocate cpu set");
    }
    size_t cpu_set_size = CPU_ALLOC_SIZE(num_cpus);
    CPU_ZERO_S(cpu_set_size, cpu_set);
    CPU_SET_S(cpu, cpu_set_size, cpu_set);

    sched_setaffinity(0, cpu_set_size, cpu_set);

    CPU_FREE(cpu_set);
}

double current_time_ms(void) {
    struct timeval time;
    gettimeofday(&time, NULL);

    return (double)time.tv_sec * 1000.0 + (double)time.tv_usec / 1000.0;
}

// Generate a uniformely distributed random number in range [min,max[
size_t uniform(size_t min, size_t max) {
    assert(min < max);
    const size_t lrand48_cardinality = 1U << 31;
    assert(max - min <= lrand48_cardinality);

    size_t rand;
    // Drop biased values
    // https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle#Modulo_bias
    const size_t first_biased =
        lrand48_cardinality - (lrand48_cardinality % (max - min));

    do {
        rand = (size_t)lrand48();
    } while (rand >= first_biased);

    return min + rand % (max - min);
}

// Generate a random permutation of the range [1,n[ using Fisher-Yates
// algorithm.
// The returned permutation is allocated on the heap.
// See: https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
size_t *random_permutation(size_t n) {
    assert(n >= 2);

    size_t *permutation = calloc(n, sizeof(size_t));
    if (permutation == NULL) {
        err(1, "Failed to allocate memory");
    }

    for (size_t i = 0; i < n; ++i) {
        permutation[i] = i;
    }

    for (size_t i = 0; i < n - 2; ++i) {
        size_t j = uniform(i, n);

        size_t tmp = permutation[i];
        permutation[i] = permutation[j];
        permutation[j] = tmp;
    }

    return permutation;
}
