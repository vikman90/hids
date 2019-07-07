/* June 23, 2019 */

#include "hids.h"

void report() {
    exit(EXIT_SUCCESS);
}

void dispatch_stdin(time_t timeout_sec) {
    char buffer[BUFFER_SIZE];
    fd_set rfds;
    struct timeval timeout = { .tv_sec = timeout_sec };

    FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO, &rfds);

    switch (select(STDIN_FILENO + 1, &rfds, NULL, NULL, &timeout)) {
    case -1:
        critical("select");

    case 0:
        break;

    default:
        switch (read(STDIN_FILENO, buffer, sizeof(buffer))) {
        case -1:
            critical("Cannot read stdin");

        case 0:
            // Agent exited. Printing a message would raise SIGPIPE.
            exit(EXIT_SUCCESS);
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
