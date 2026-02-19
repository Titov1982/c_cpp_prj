#pragma once

#include <stdio.h>
#include <stdint.h>

struct RandomGenerator;

struct RandomGeneratorOps {
    void (*init)(struct RandomGenerator *g, const char *paramas);
    void (*destroy)(struct RandomGenerator *g);
    int32_t (*next_int32)(struct RandomGenerator *g);
};

struct RandomGenerator {
    
    const struct RandomGeneratorOps *ops;

};

void rand_init(struct RandomGenerator *g, const char *params);
void rand_destroy(struct RandomGenerator *g);
int32_t rand_next_int32(struct RandomGenerator *g);
