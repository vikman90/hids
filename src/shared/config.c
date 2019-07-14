/* June 29, 2019 */

#include "hids.h"

static void parse_root(yaml_document_t * document, const char * module, void (* routine)(yaml_document_t * document, yaml_node_t * node));

int parse_config(const char * module, void (* routine)(yaml_document_t * document, yaml_node_t * node)) {
    int error = 0;
    FILE * file = fopen(CONFIG_FILE, "r");

    if (file == NULL) {
        print_error("Cannot open configuration file: %s", strerror(errno));
        return -1;
    }

    yaml_document_t document;
    yaml_parser_t parser;

    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, file);

    if (yaml_parser_load(&parser, &document)) {
        parse_root(&document, module, routine);
        yaml_document_delete(&document);
    } else {
        print_error("Invalid syntax at %s:%lu", CONFIG_FILE, parser.problem_mark.line);
        error = -1;
    }

    yaml_parser_delete(&parser);
    fclose(file);
    return error;
}

void parse_root(yaml_document_t * document, const char * module, void (* routine)(yaml_document_t * document, yaml_node_t * node)) {
    yaml_node_t * root = yaml_document_get_root_node(document);

    if (root == NULL) {
        print_info("Configuration is empty");
        return;
    }

    assert_node_mapping(root);

    for mapping(i, root) {
        yaml_node_t * key = yaml_document_get_node(document, i->key);
        yaml_node_t * value = yaml_document_get_node(document, i->value);
        char * skey = scalar(key);

        if (strcmp(skey, module) == 0) {
            routine(document, value);
        }
    }
}
