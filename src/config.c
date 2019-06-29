/* June 29, 2019 */

#include "tree.h"

#define mapping(i, node) (yaml_node_pair_t * i = node->data.mapping.pairs.start; i < node->data.mapping.pairs.top; ++i)
#define sequence(i, node) (yaml_node_item_t * i = node->data.sequence.items.start; i < node->data.sequence.items.top; ++i)

#define scalar(node) (char *)node->data.scalar.value
#define line(node) node->start_mark.line

static int parse_root(yaml_document_t * document);
static int parse_logcollector(yaml_document_t * document, yaml_node_t * node);
static void add_logfile(const char * file);

int parse_config() {
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
        error = parse_root(&document);
        yaml_document_delete(&document);
    } else {
        print_error("Invalid syntax at %s:%lu", CONFIG_FILE, parser.problem_mark.line);
        error = -1;
    }

    yaml_parser_delete(&parser);
    fclose(file);
    return error;
}

int parse_root(yaml_document_t * document) {
    yaml_node_t * root = yaml_document_get_root_node(document);

    if (root == NULL) {
        print_info("Configuration is empty");
        return 0;
    }

    if (root->type != YAML_MAPPING_NODE) {
        print_error("Invalid node at %s:%lu", CONFIG_FILE, line(root));
        return -1;
    }

    for mapping(i, root) {
        yaml_node_t * key = yaml_document_get_node(document, i->key);
        yaml_node_t * value = yaml_document_get_node(document, i->value);
        char * skey = scalar(key);

        if (strcmp(skey, "logcollector") == 0) {
            if (parse_logcollector(document, value) != 0) {
                return -1;
            }
        } else {
            print_warn("Unknown element '%s' at %s:%lu", skey, CONFIG_FILE, line(key));
        }
    }

    return 0;
}

int parse_logcollector(yaml_document_t * document, yaml_node_t * node) {
    if (node->type != YAML_MAPPING_NODE) {
        print_error("Invalid node at %s:%lu", CONFIG_FILE, line(node));
        return -1;
    }

    for mapping(i, node) {
        yaml_node_t * key = yaml_document_get_node(document, i->key);
        yaml_node_t * value = yaml_document_get_node(document, i->value);
        char * skey = scalar(key);

        if (strcmp(skey, "files") == 0) {
            for sequence(i, value) {
                yaml_node_t * item = yaml_document_get_node(document, *i);

                if (item->type != YAML_SCALAR_NODE) {
                    print_warn("Invalid node at %s:%lu", CONFIG_FILE, line(item));
                }

                add_logfile(scalar(item));
            }
        } else {
            print_warn("Unknown element '%s' at %s:%lu", skey, CONFIG_FILE, line(key));
        }
    }

    return 0;
}

void add_logfile(const char * file) {
    for (unsigned i = 0; i < logcollector.length; i++) {
        if (strcmp(logcollector.logfiles[i].path, file) == 0) {
            print_warn("Duplicate logcollector stanza: %s", file);
            return;
        }
    }

    unsigned last = logcollector.length++;
    logcollector.logfiles = realloc(logcollector.logfiles, logcollector.length * sizeof(logfile_t));
    memset(logcollector.logfiles + last, 0, sizeof(logfile_t));

    if (logcollector.logfiles == NULL) {
        critical("Cannot allocate memory for log files");
    }

    logcollector.logfiles[last].path = strdup(file);
}
