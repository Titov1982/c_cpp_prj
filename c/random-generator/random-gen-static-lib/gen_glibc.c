#include "rand_interface.h"
#include "gen_glibc.h"

#include <stdlib.h>

struct RandomGeneratorGlibc {

    struct RandomGenerator b;
    
    struct random_data rd;

};

static void init_func(struct RandomGenerator *g, const char *params) {
    
    // struct RandomGeneratorGlibc *gg = (struct RandomGeneratorGlibc*) g;
    unsigned int seed = strtoul(params, NULL, 10);
    // srandom_r(seed, &gg->rd);    
    srandom(seed);
}
static void destroy_func(struct RandomGenerator *g) {

}
static int32_t next_int32_func(struct RandomGenerator *g) {
   
    // struct RandomGeneratorGlibc *gg = (struct RandomGeneratorGlibc*) g;
    // int32_t res;
    // random_r(&gg->rd, &res);
    // return res;
    return random();
}

static const struct RandomGeneratorOps oper_glibc = {
    
    .init = init_func,
    .destroy = destroy_func,
    .next_int32 = next_int32_func,
};

struct RandomGenerator* rand_create_glibc(void) {

    struct RandomGeneratorGlibc *rg = NULL;
    rg = calloc(1, sizeof(*rg));
    rg->b.ops = &oper_glibc;
    return &rg->b;
}
