#include <assert.h>
#include <err.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>

#include "utils.h"

void usage(const char *program_name);
void parse_arguments(int argc, char *argv[], size_t *n, bool *random_layout);

typedef struct node node_t;
struct node {
    node_t *next;
    unsigned int val;
};

size_t __attribute__((noinline)) list_sum(const node_t *head) {
    size_t sum = 0;

    for (const node_t *curr = head; curr != NULL; curr = curr->next) {
        sum += curr->val;
    }

    return sum;
}

// Create a linked list with a random layout
node_t *list_random_layout(size_t n) {
    size_t *permutation = random_permutation(n);

    node_t *nodes = calloc(n, sizeof(node_t));
    if (nodes == NULL) {
        err(1, "Failed to allocate memory");
    }

    node_t *prev = &nodes[permutation[0]];
    for (size_t i = 1; i < n - 1; ++i) {
        prev->next = &nodes[permutation[i]];

        prev = prev->next;
    }
    prev->next = NULL;

    free(permutation);

    return nodes;
}

// Create a linked list with a sequential layout
node_t *list_sequential_layout(size_t n) {
    node_t *nodes = calloc(n, sizeof(node_t));
    if (nodes == NULL) {
        err(1, "Failed to allocate memory");
    }

    node_t *prev = &nodes[0];
    for (size_t i = 1; i < n - 1; ++i) {
        prev->next = &nodes[i];

        prev = prev->next;
    }
    prev->next = NULL;

    return nodes;
}

int main(int argc, char *argv[]) {
    size_t n;
    bool random_layout;
    parse_arguments(argc, argv, &n, &random_layout);

    // Pin the process to cpu 0 to avoid cache miss if the process is move
    // to another cpu
    pin_to_cpu(0);
    // Need permissions
    // if (setpriority(PRIO_PROCESS, (unsigned int)getpid(), -20) == -1) {
    //     err(1, "Failed to set process priority");
    // }

    // Seed the PRNG
    srand48(1);

    node_t *head =
        random_layout ? list_random_layout(n) : list_sequential_layout(n);

    // Initialize values (use small values to avoid overflow)
    for (node_t *curr = head; curr != NULL; curr = curr->next) {
        curr->val = (unsigned int)uniform(0UL, 4UL);
    }

    // Warm up caches
    BLACK_BOX(list_sum(head));

    size_t num_runs = 10;
    printf("Running list_sum %zu times\n", num_runs);

    double total_time = 0.;
    for (size_t i = 0; i < num_runs; ++i) {
        double run_start_time = current_time_ms();

        BLACK_BOX(list_sum(head));

        total_time += current_time_ms() - run_start_time;
    }

    printf("Total time: %lf ms\n", total_time);
    printf("Mean time: %lf ms\n", total_time / (double)num_runs);

    free(head);

    return 0;
}

void usage(const char *program_name) {
    fprintf(stderr,
            "Usage: %s N LAYOUT\n\n"
            "Arguments:\n"
            "  N       Number of linked list elements\n"
            "  LAYOUT  Linked list layout (random or sequential)\n",
            program_name);
}

void parse_arguments(int argc, char *argv[], size_t *n, bool *random_layout) {
    if (argc != 3) {
        goto invalid_args;
    }

    char *endptr;
    long x = strtol(argv[1], &endptr, 10);
    if (argv[1][0] == '\0' || endptr[0] != '\0' || x < 1 || x == LONG_MAX) {
        fprintf(stderr, "Invalid argument N\n");
        goto invalid_args;
    }
    *n = (size_t)x;

    if (strcmp(argv[2], "random") == 0) {
        *random_layout = true;
    } else if (strcmp(argv[2], "sequential") == 0) {
        *random_layout = false;
    } else {
        fprintf(stderr, "Invalid argument LAYOUT\n");
        goto invalid_args;
    }

    return;

invalid_args:
    usage(argv[0]);
    exit(1);
}
