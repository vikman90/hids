/* July 14, 2019 */

#include "hids.h"

static void add_logfile(const char * file);

void parse_logcollector(yaml_document_t * document, yaml_node_t * node) {
    assert_node_mapping(node);

    for mapping(i, node) {
        yaml_node_t * key = yaml_document_get_node(document, i->key);
        yaml_node_t * value = yaml_document_get_node(document, i->value);
        char * skey = scalar(key);

        if (strcmp(skey, "files") == 0) {
            if (value->type != YAML_SEQUENCE_NODE) {
                print_invalid(value);
                continue;
            }

            for sequence(i, value) {
                yaml_node_t * item = yaml_document_get_node(document, *i);

                if (item->type != YAML_SCALAR_NODE) {
                    print_invalid(value);
                    continue;
                }

                add_logfile(scalar(item));
            }
        } else {
            print_unknown(key);
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
