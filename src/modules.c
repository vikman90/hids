/* June 23, 2019 */

#include "hids.h"

void report() {
    exit(EXIT_SUCCESS);
}

void dispatch_socket(time_t timeout_sec) {
    char buffer[BUFFER_SIZE];
    fd_set rfds;
    struct timeval timeout = { .tv_sec = timeout_sec };

    FD_ZERO(&rfds);
    FD_SET(cur_module->sock, &rfds);

    switch (select(cur_module->sock + 1, &rfds, NULL, NULL, &timeout)) {
    case -1:
        critical("select");

    case 0:
        break;

    default:
        switch (read(cur_module->sock, buffer, sizeof(buffer))) {
        case -1:
            critical("Cannot read socket");

        case 0:
            print_info("Agent exited. Closing");
            exit(EXIT_SUCCESS);

        default:
            print_warn("Unexpected data received via socket");
        }
    }
}

int sca_main() {
    report();
    return EXIT_SUCCESS;
}

int openscap_main() {
    report();
    return EXIT_SUCCESS;
}

int osquery_main() {
    report();
    return EXIT_SUCCESS;
}

int rootcheck_main() {
    report();
    return EXIT_SUCCESS;
}

int ciscat_main() {
    report();
    return EXIT_SUCCESS;
}

int inventory_main() {
    report();
    return EXIT_SUCCESS;
}

int aws_main() {
    report();
    return EXIT_SUCCESS;
}

int azure_main() {
    report();
    return EXIT_SUCCESS;
}

int docker_main() {
    report();
    return EXIT_SUCCESS;
}

int command_main() {
    report();
    return EXIT_SUCCESS;
}
