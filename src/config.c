/* June 29, 2019 */

#include "hids.h"

#define mapping(i, node) (yaml_node_pair_t * i = node->data.mapping.pairs.start; i < node->data.mapping.pairs.top; ++i)
#define sequence(i, node) (yaml_node_item_t * i = node->data.sequence.items.start; i < node->data.sequence.items.top; ++i)

#define scalar(node) (char *)node->data.scalar.value
#define line(node) node->start_mark.line

#define assert_node_mapping(node) if (node->type != YAML_MAPPING_NODE) { print_error("Invalid node at %s:%lu", CONFIG_FILE, line(node)); return; }

static void parse_root(yaml_document_t * document);
static void parse_logcollector(yaml_document_t * document, yaml_node_t * node);
static void add_logfile(const char * file);
static void parse_fim(yaml_document_t * document, yaml_node_t * node);
static void add_fim(const char * path);

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
        parse_root(&document);
        yaml_document_delete(&document);
    } else {
        print_error("Invalid syntax at %s:%lu", CONFIG_FILE, parser.problem_mark.line);
        error = -1;
    }

    yaml_parser_delete(&parser);
    fclose(file);
    return error;
}

void parse_root(yaml_document_t * document) {
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

        if (strcmp(skey, "logcollector") == 0) {
            parse_logcollector(document, value);
        } else if (strcmp(skey, "fim") == 0) {
            parse_fim(document, value);
        } else {
            print_warn("Unknown element '%s' at %s:%lu", skey, CONFIG_FILE, line(key));
        }
    }
}

void parse_logcollector(yaml_document_t * document, yaml_node_t * node) {
    assert_node_mapping(node);

    for mapping(i, node) {
        yaml_node_t * key = yaml_document_get_node(document, i->key);
        yaml_node_t * value = yaml_document_get_node(document, i->value);
        char * skey = scalar(key);

        if (strcmp(skey, "files") == 0) {
            for sequence(i, value) {
                yaml_node_t * item = yaml_document_get_node(document, *i);

                if (item->type != YAML_SCALAR_NODE) {
                    print_warn("Invalid node at %s:%lu", CONFIG_FILE, line(item));
                    continue;
                }

                add_logfile(scalar(item));
            }
        } else {
            print_warn("Unknown element '%s' at %s:%lu", skey, CONFIG_FILE, line(key));
        }
    }
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

void parse_fim(yaml_document_t * document, yaml_node_t * node) {
    assert_node_mapping(node);

    for mapping(i, node) {
        yaml_node_t * key = yaml_document_get_node(document, i->key);
        yaml_node_t * value = yaml_document_get_node(document, i->value);
        char * skey = scalar(key);

        if (strcmp(skey, "items") == 0) {
            for sequence(i, value) {
                yaml_node_t * item = yaml_document_get_node(document, *i);

                if (item->type != YAML_SCALAR_NODE) {
                    print_warn("Invalid node at %s:%lu", CONFIG_FILE, line(item));
                    continue;
                }

                add_fim(scalar(item));
            }
        } else if (strcmp(skey, "follow_links") == 0) {
            if (value->type != YAML_SCALAR_NODE) {
                print_warn("Invalid node at %s:%lu", CONFIG_FILE, line(value));
                continue;
            }

            char *svalue = scalar(value);

            if (strcmp(svalue, "yes") == 0) {
                fim.follow_links = 1;
            } else if (strcmp(svalue, "no") == 0) {
                fim.follow_links = 0;
            } else {
                print_warn("Invalid value '%s' at %s:%lu", svalue, CONFIG_FILE, line(value));
            }
        } else if (strcmp(skey, "size_limit") == 0) {
            char * svalue = scalar(value);
            char * end;
            long s = strtol(svalue, &end, 10);

            if (s == LONG_MIN || s == LONG_MAX) {
                print_warn("Invalid value '%s' at %s:%lu", svalue, CONFIG_FILE, line(value));
            } else {
                switch (*end) {
                case '\0':
                    fim.size_limit = s;
                    break;
                case 'K':
                    fim.size_limit = s * 1024;
                    break;
                case 'M':
                    fim.size_limit = s * 1048576;
                    break;
                case 'G':
                    fim.size_limit = s * 1073741824;
                    break;
                default:
                    print_warn("Invalid value '%s' at %s:%lu", svalue, CONFIG_FILE, line(value));
                }
            }
        } else if (strcmp(skey, "max_files") == 0) {
            char * svalue = scalar(value);
            char * end;
            unsigned long max = strtoul(svalue, &end, 10);

            if (max == ULONG_MAX || *end) {
                print_warn("Invalid value '%s' at %s:%lu", svalue, CONFIG_FILE, line(value));
            }

            fim.max_files = max;
        } else {
            print_warn("Unknown element '%s' at %s:%lu", skey, CONFIG_FILE, line(key));
        }
    }
}

void add_fim(const char * path) {
    for (unsigned i = 0; i < fim.length; i++) {
        if (strcmp(fim.items[i].path, path) == 0) {
            print_warn("Duplicate FIM stanza: %s", path);
            return;
        }
    }

    unsigned last = fim.length++;
    fim.items = realloc(fim.items, fim.length * sizeof(fim_item_t));
    memset(fim.items + last, 0, sizeof(fim_item_t));

    fim.items[last].path = strdup(path);
}
