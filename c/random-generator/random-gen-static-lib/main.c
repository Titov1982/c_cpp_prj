#include "rand_interface.h"
#include "gen_glibc.h"

#include <stdio.h>
#include <string.h>

int main (int argc, char* argv[]) {

    const char* name = argv[1];
    const char* params = argv[2];

    struct RandomGenerator *r = NULL;
    if (!strcmp(name, "glibc")) {
        r = rand_create_glibc();
    }

    rand_init(r, params);
    for (int i = 0; i < 10; i++) {
        printf("%d\n", rand_next_int32(r));
    }
    rand_destroy(r);

}
