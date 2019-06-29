/* June 21, 2019 */

#include "tree.h"

int main() {

    // Set up disposer
    set_handler(SIGINT, handler);
    set_handler(SIGTERM, handler);

    // Fork modules

    for (module_t * m = modules; m->name; m++) {
        spawn(m);
    }

    print_info("agent", "Started");

    // Watchdog

    for (;;) {
        dispatch();
    }

    return EXIT_SUCCESS;
}
