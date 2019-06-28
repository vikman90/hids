/* June 23, 2019 */

#include "tree.h"

void report(const char * name, int sock) {
    setlinebuf(stdout);

    while (1) {
        unsigned int x;
        getrandom(&x, sizeof(x), 0);
        x = (double)x / UINT_MAX * 50;

        switch (x) {
        case 0:
            print_error(name, "This module should be disabled. Stopping");
            exit(EXIT_FAILURE);

        case 1:
            print_error(name, "Random issue. Stopping");
            abort();
        }

        printf("%u\n", x);

        // Wait socket

        fd_set rfds;
        struct timeval timeout = { .tv_sec = 5 };
        char buffer[1024];

        FD_ZERO(&rfds);
        FD_SET(sock, &rfds);

        switch (select(sock + 1, &rfds, NULL, NULL, &timeout)) {
        case -1:
            critical(name, "select");

        case 0:
            break;

        default:
            switch (read(sock, buffer, sizeof(buffer))) {
            case -1:
                critical(name, "Cannot read socket");

            case 0:
                print_info(name, "Agent exited. Closing");
                exit(EXIT_SUCCESS);

            default:
                print_warn(name, "Unexpected data received via socket");
            }
        }
    }
}

int fim_main(module_t * module) {
    report(module->name, module->sock);
    return EXIT_SUCCESS;
}

int logcollector_main(module_t * module) {
    report(module->name, module->sock);
    return EXIT_SUCCESS;
}

int sca_main(module_t * module) {
    report(module->name, module->sock);
    return EXIT_SUCCESS;
}

int openscap_main(module_t * module) {
    report(module->name, module->sock);
    return EXIT_SUCCESS;
}

int osquery_main(module_t * module) {
    report(module->name, module->sock);
    return EXIT_SUCCESS;
}

int rootcheck_main(module_t * module) {
    report(module->name, module->sock);
    return EXIT_SUCCESS;
}

int ciscat_main(module_t * module) {
    report(module->name, module->sock);
    return EXIT_SUCCESS;
}

int inventory_main(module_t * module) {
    report(module->name, module->sock);
    return EXIT_SUCCESS;
}

int aws_main(module_t * module) {
    report(module->name, module->sock);
    return EXIT_SUCCESS;
}

int azure_main(module_t * module) {
    report(module->name, module->sock);
    return EXIT_SUCCESS;
}

int docker_main(module_t * module) {
    report(module->name, module->sock);
    return EXIT_SUCCESS;
}

int command_main(module_t * module) {
    report(module->name, module->sock);
    return EXIT_SUCCESS;
}
