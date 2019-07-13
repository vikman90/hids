/* July 23, 2019 */

#include "hids.h"

module_t modules[] = {
    { .name = NULL, NULL }
};

static module_t manager = { .name = "manager" };
module_t * cur_module = &manager;

const char * argv0;

int main(__attribute__((unused)) int argc, char ** argv) {
    argv0 = argv[0];
    return 0;
}
