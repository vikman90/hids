// July 26, 2019

#include <shared.h>

Config::Config(const char * path) {
    File file(path);
    Parser parser(file);

    try {
        parser.load(document);
    } catch (SyntaxException &e) {
        ostringstream stream;
        stream << "Invalid syntax at " << path << ":" << e.getLine();
        throw Exception(HERE, stream.str());
    }
}

Config::~Config() {
    yaml_document_delete(&document);
}

yaml_node_t * Config::root() {
    yaml_node_t * root = yaml_document_get_root_node(&document);

    if (root == NULL) {
        throw EmptyException();
    }

    return root;
}

yaml_node_t * Config::operator [] (int index) {
    return yaml_document_get_node(&document, index);
}

Parser::Parser(File & file) {
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, file.getPointer());
}

Parser::~Parser() {
    yaml_parser_delete(&parser);
}

void Parser::load(yaml_document_t & document) {
    if (!yaml_parser_load(&parser, &document)) {
        throw SyntaxException(parser.problem_mark.line);
    }
}

Scalar::Scalar(yaml_node_t * node) : node(node) {
    if (node->type != YAML_SCALAR_NODE) {
        throw SemanticException(node->start_mark.line, "Expecting a map");
    }
}

Scalar::operator char * () {
    return (char *)node->data.scalar.value;
}

Scalar::operator string () {
    return (char *)node->data.scalar.value;
}

Scalar::operator long () {
    char * end;
    long value = strtol((char *)node->data.scalar.value, &end, 10);

    if (*end) {
        throw SemanticException(node->start_mark.line, "Expecting an integer");
    }

    return value;
}

Mapping::Mapping(yaml_node_t * node) : node(node) {
    if (node->type != YAML_MAPPING_NODE) {
        throw SemanticException(node->start_mark.line, "Expecting a map");
    }
}

yaml_node_pair_t * Mapping::begin() {
    return node->data.mapping.pairs.start;
}

yaml_node_pair_t * Mapping::end() {
    return node->data.mapping.pairs.top;
}

Sequence::Sequence(yaml_node_t * node) : node(node) {
    if (node->type != YAML_SEQUENCE_NODE) {
        throw SemanticException(node->start_mark.line, "Expecting a sequence");
    }
}

yaml_node_item_t * Sequence::begin() {
    return node->data.sequence.items.start;
}

yaml_node_item_t * Sequence::end() {
    return node->data.sequence.items.top;
}

int ConfigException::getLine() {
    return line;
}
