/* July 14, 2019 */

#include "hids.h"

static void add_fim(const char * path);

void parse_fim(yaml_document_t * document, yaml_node_t * node) {
    assert_node_mapping(node);

    for mapping(i, node) {
        yaml_node_t * key = yaml_document_get_node(document, i->key);
        yaml_node_t * value = yaml_document_get_node(document, i->value);
        char * skey = scalar(key);

        if (strcmp(skey, "items") == 0) {
            if (value->type != YAML_SEQUENCE_NODE) {
                print_invalid(value);
                continue;
            }

            for sequence(i, value) {
                yaml_node_t * item = yaml_document_get_node(document, *i);

                if (item->type != YAML_SCALAR_NODE) {
                    print_invalid(item);
                    continue;
                }

                add_fim(scalar(item));
            }
        } else if (strcmp(skey, "follow_links") == 0) {
            if (value->type != YAML_SCALAR_NODE) {
                print_invalid(value);
                continue;
            }

            char *svalue = scalar(value);

            if (strcmp(svalue, "yes") == 0) {
                fim.follow_links = 1;
            } else if (strcmp(svalue, "no") == 0) {
                fim.follow_links = 0;
            } else {
                print_invalid(value);
            }
        } else if (strcmp(skey, "real_time") == 0) {
            if (value->type != YAML_SCALAR_NODE) {
                print_warn("Invalid node at %s:%lu", CONFIG_FILE, line(value));
                continue;
            }

            char *svalue = scalar(value);

            if (strcmp(svalue, "yes") == 0) {
                fim.real_time = 1;
            } else if (strcmp(svalue, "no") == 0) {
                fim.real_time = 0;
            } else {
                print_invalid(value);
            }
        } else if (strcmp(skey, "size_limit") == 0) {
            char * svalue = scalar(value);
            char * end;
            long s = strtol(svalue, &end, 10);

            if (s == LONG_MIN || s == LONG_MAX) {
                print_invalid(value);
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
                    print_invalid(value);
                }
            }
        } else if (strcmp(skey, "max_files") == 0) {
            char * svalue = scalar(value);
            char * end;
            unsigned long max = strtoul(svalue, &end, 10);

            if (max == ULONG_MAX || *end) {
                print_invalid(value);
            }

            fim.max_files = max;
        } else {
            print_unknown(key);
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
