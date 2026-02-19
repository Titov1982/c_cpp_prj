#include "rand_interface.h"

/*
    void (*init)(struct RandomGenerator *g, const char *params),
    void (*destroy)(struct RandomGenerator *g),
    int32_t (*next_int32)(struct RandomGenerator *g)
 */

void rand_init(struct RandomGenerator *g, const char *params) {
    g->ops->init(g, params);

}

void rand_destroy(struct RandomGenerator *g) {
    g->ops->destroy(g);
}

int32_t rand_next_int32(struct RandomGenerator *g) {
    return g->ops->next_int32(g);
}
