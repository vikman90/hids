/* July 14, 2019 */

#ifndef CONFIG_H
#define CONFIG_H

#define mapping(i, node) (yaml_node_pair_t * i = node->data.mapping.pairs.start; i < node->data.mapping.pairs.top; ++i)
#define sequence(i, node) (yaml_node_item_t * i = node->data.sequence.items.start; i < node->data.sequence.items.top; ++i)

#define scalar(node) (char *)node->data.scalar.value
#define line(node) node->start_mark.line

#define print_invalid(node) print_warn("Invalid node at %s:%lu", CONFIG_FILE, line(node))
#define print_unknown(node) print_warn("Unknown element '%s' at %s:%lu", scalar(node), CONFIG_FILE, line(node))

#define assert_node_mapping(node) if (node->type != YAML_MAPPING_NODE) { print_invalid(node); return; }

int parse_config(const char * module, void (* routine)(yaml_document_t * document, yaml_node_t * node));
void parse_logcollector(yaml_document_t * document, yaml_node_t * node);
void parse_fim(yaml_document_t * document, yaml_node_t * node);
void parse_client(yaml_document_t * document, yaml_node_t * node);

#endif
