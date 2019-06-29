/* June 21, 2019 */

#include "tree.h"

int main(__attribute__((unused)) int argc, char ** argv) {

    // Set up disposer
    set_handler(SIGINT, handler);
    set_handler(SIGTERM, handler);

    set_cwd(argv[0]);

    // Read configuration

    if (parse_config() != 0) {
        return EXIT_FAILURE;
    }

    // Fork modules

    for (module_t * m = modules; m->name; m++) {
        spawn(m);
    }

    print_info("Started");

    // Watchdog

    for (;;) {
        dispatch();
    }

    return EXIT_SUCCESS;
}
