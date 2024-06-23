// June 29, 2019
// June 23, 2024

#include <logger.hpp>
#include <logcollector.hpp>
#include <cstdlib>
#include <climits>
#include "../include/config_parser.hpp"

#define mapping(i, node) (yaml_node_pair_t * i = node->data.mapping.pairs.start; i < node->data.mapping.pairs.top; ++i)
#define sequence(i, node) (yaml_node_item_t * i = node->data.sequence.items.start; i < node->data.sequence.items.top; ++i)

#define scalar(node) (char *)node->data.scalar.value
#define line(node) node->start_mark.line

#define assert_node_mapping(node) if (node->type != YAML_MAPPING_NODE) { Logger::error("Invalid node at {}:{}}", configFilePath, line(node)); return; }

int ConfigParser::parse() {
    int error = 0;
    FILE * file = fopen(configFilePath.c_str(), "r");

    if (file == nullptr) {
        Logger::error("Cannot open configuration file: {}", strerror(errno));
        return -1;
    }

    yaml_parser_t parser;

    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, file);

    if (yaml_parser_load(&parser, &document)) {
        parseRoot();
        yaml_document_delete(&document);
    } else {
        Logger::error("Invalid syntax at {}:{}", configFilePath, parser.problem_mark.line);
        error = -1;
    }

    yaml_parser_delete(&parser);
    fclose(file);
    return error;
}

void ConfigParser::parseRoot() {
    yaml_node_t * root = yaml_document_get_root_node(&document);

    if (root == nullptr) {
        Logger::info("Configuration is empty");
        return;
    }

    assert_node_mapping(root);

    for mapping(i, root) {
        yaml_node_t * key = yaml_document_get_node(&document, i->key);
        yaml_node_t * value = yaml_document_get_node(&document, i->value);
        char * skey = scalar(key);

        if (strcmp(skey, "logcollector") == 0) {
            parseLogcollector(value);
        } else if (strcmp(skey, "fim") == 0) {
            // parseFIM(value);
        } else {
            Logger::warn("Unknown element '{}' at {}:{}", skey, configFilePath, line(key));
        }
    }
}

void ConfigParser::parseLogcollector(yaml_node_t * node) {
    assert_node_mapping(node);
    Logcollector & logcollector = Logcollector::getInstance();

    for mapping(i, node) {
        yaml_node_t * key = yaml_document_get_node(&document, i->key);
        yaml_node_t * value = yaml_document_get_node(&document, i->value);
        char * skey = scalar(key);

        if (strcmp(skey, "files") == 0) {
            for sequence(i, value) {
                yaml_node_t * item = yaml_document_get_node(&document, *i);

                if (item->type != YAML_SCALAR_NODE) {
                    Logger::warn("Invalid node at {}:{}", configFilePath, line(item));
                    continue;
                }

                logcollector.addLogfile(scalar(item));
            }
        } else {
            Logger::warn("Unknown element '{}' at {}:{}", skey, configFilePath, line(key));
        }
    }
}
