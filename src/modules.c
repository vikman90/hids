/* June 23, 2019 */

#include "tree.h"

void report() {

    set_name(cur_module->name);

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
    struct timeval timeout = { .tv_sec = 5 };

    FD_ZERO(&rfds);
    FD_SET(cur_module->sock, &rfds);

    switch (select(cur_module->sock + 1, &rfds, NULL, NULL, &timeout)) {
    case -1:
        critical(cur_module->name, "select");

    case 0:
        break;

    default:
        switch (read(cur_module->sock, buffer, sizeof(buffer))) {
        case -1:
            critical(cur_module->name, "Cannot read socket");

        case 0:
            print_info(cur_module->name, "Agent exited. Closing");
            exit(EXIT_SUCCESS);

        default:
            print_warn(cur_module->name, "Unexpected data received via socket");
        }
    }
}

int fim_main(module_t * module) {
    cur_module = module;
    report();
    return EXIT_SUCCESS;
}

int logcollector_main(module_t * module) {
    cur_module = module;
    report();
    return EXIT_SUCCESS;
}

int sca_main(module_t * module) {
    cur_module = module;
    report();
    return EXIT_SUCCESS;
}

int openscap_main(module_t * module) {
    cur_module = module;
    report();
    return EXIT_SUCCESS;
}

int osquery_main(module_t * module) {
    cur_module = module;
    report();
    return EXIT_SUCCESS;
}

int rootcheck_main(module_t * module) {
    cur_module = module;
    report();
    return EXIT_SUCCESS;
}

int ciscat_main(module_t * module) {
    cur_module = module;
    report();
    return EXIT_SUCCESS;
}

int inventory_main(module_t * module) {
    cur_module = module;
    report();
    return EXIT_SUCCESS;
}

int aws_main(module_t * module) {
    cur_module = module;
    report();
    return EXIT_SUCCESS;
}

int azure_main(module_t * module) {
    cur_module = module;
    report();
    return EXIT_SUCCESS;
}

int docker_main(module_t * module) {
    cur_module = module;
    report();
    return EXIT_SUCCESS;
}

int command_main(module_t * module) {
    cur_module = module;
    report();
    return EXIT_SUCCESS;
}
