#ifndef UTILS_H_
#define UTILS_H_

#include <stddef.h>

// Prevent the compiler from optimizing away the expression
#define BLACK_BOX(...)                                                         \
    do {                                                                       \
        __attribute__((unused)) volatile __auto_type _res = (__VA_ARGS__);     \
    } while (0)

void pin_to_cpu(size_t cpu);

double current_time_ms(void);

size_t uniform(size_t min, size_t max);
size_t *random_permutation(size_t n);

#endif /* UTILS_H_ */
