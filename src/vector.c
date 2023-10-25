#include <err.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "utils.h"

void usage(const char *program_name);
void parse_arguments(int argc, char *argv[], size_t *n);

typedef struct {
    unsigned int *buf;
    size_t capacity;
    size_t len;
} vec_t;

size_t __attribute__((noinline)) vec_sum(const vec_t *vec) {
    size_t sum = 0;

    for (size_t i = 0; i < vec->len; ++i) {
        sum += vec->buf[i];
    }

    return sum;
}

int main(int argc, char *argv[]) {
    size_t n;
    parse_arguments(argc, argv, &n);

    // Pin the process to cpu 0 to avoid cache miss if the process is move
    // to another cpu
    pin_to_cpu(0);
    // Need permissions
    // if (setpriority(PRIO_PROCESS, (unsigned int)getpid(), -20) == -1) {
    //     err(1, "Failed to set process priority");
    // }

    // Seed the PRNG
    srand48(1);

    vec_t vec = { .capacity = n, .len = n };

    vec.buf = calloc(n, sizeof(unsigned int));
    if (vec.buf == NULL) {
        err(1, "Failed to allocate memory");
    }

    // Initialize values (use small values to avoid overflow)
    for (size_t i = 0; i < n; ++i) {
        vec.buf[i] = (unsigned int)uniform(0UL, 4UL);
    }

    // Warm up caches
    BLACK_BOX(vec_sum(&vec));

    size_t num_runs = 10;
    printf("Running list_sum %zu times\n", num_runs);

    double total_time = 0.;
    for (size_t i = 0; i < num_runs; ++i) {
        double run_start_time = current_time_ms();

        BLACK_BOX(vec_sum(&vec));

        total_time += current_time_ms() - run_start_time;
    }

    printf("Total time: %lf ms\n", total_time);
    printf("Mean time: %lf ms\n", total_time / (double)num_runs);

    free(vec.buf);

    return 0;
}

void usage(const char *program_name) {
    fprintf(stderr,
            "Usage: %s N\n\n"
            "Arguments:\n"
            "  N       Number of linked list elements\n",
            program_name);
}

void parse_arguments(int argc, char *argv[], size_t *n) {
    if (argc != 2) {
        goto invalid_args;
    }

    char *endptr;
    long x = strtol(argv[1], &endptr, 10);
    if (argv[1][0] == '\0' || endptr[0] != '\0' || x < 1 || x == LONG_MAX) {
        fprintf(stderr, "Invalid argument N\n");
        goto invalid_args;
    }
    *n = (size_t)x;

    return;

invalid_args:
    usage(argv[0]);
    exit(1);
}
