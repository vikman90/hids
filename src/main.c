/* June 21, 2019 */

#include "hids.h"

static const char * config_path;
const char * argv0;

int main(int argc, char ** argv) {
    argv0 = argv[0];

    set_cwd();

    // Parse options
    int opt;

    while ((opt = getopt(argc, argv, "c:")) != -1) {
        switch (opt) {
        case 'c':
            config_path = optarg;
            break;
        case '?':
            break;
        }
    }

    // Read configuration
    if (parse_config(config_path) != 0) {
        return EXIT_FAILURE;
    }

    // Set up disposer
    set_handler(SIGINT, handler);
    set_handler(SIGTERM, handler);

    // Fork modules

    if (optind < argc) {
        // Launch a single module
        for (module_t * m = modules; m->name; m++) {
            if (strcmp(argv[optind], m->name) == 0) {
                cur_module = m;
                m->sock = 3;
                cloexec(m->sock);
                setlinebuf(stdout);
                exit(m->main());
            }
        }

        print_error("No such module '%s'", argv[optind]);
        exit(EXIT_FAILURE);
    }

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
