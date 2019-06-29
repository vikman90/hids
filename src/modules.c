/* June 23, 2019 */

#include "tree.h"

void report() {
    while (1) {
        unsigned int x;
        getrandom(&x, sizeof(x), 0);
        x = (double)x / UINT_MAX * 50;

        const char * MESSAGE = "Hello world.";
        cJSON * root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "message", MESSAGE);
        cJSON_AddNumberToObject(root, "data", x);
        char * payload = cJSON_PrintUnformatted(root);
        printf("%s\n", payload);
        free(payload);
        cJSON_Delete(root);

        dispatch_socket();
    }
}

void dispatch_socket() {
    char buffer[BUFFER_SIZE];
    fd_set rfds;
    struct timeval timeout = { .tv_sec = 60 };

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

int fim_main() {
    report();
    return EXIT_SUCCESS;
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
