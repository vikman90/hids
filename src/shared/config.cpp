// July 26, 2019

#include "config.h"

Config::~Config() {
    yaml_document_delete(&document);
}

void Config::read(const char * path) {
    File file(path);
    Parser parser(file);

    try {
        parser.load(document);
    } catch (Parser::Exception &e) {
        ostringstream stream;
        stream << "Invalid syntax at " << path << ":" << e.getLine();
        throw Exception(HERE, stream.str());
    }
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
        throw Exception(parser.problem_mark.line);
    }
}

Parser::Exception::Exception(int line) : line(line) { }

int Parser::Exception::getLine() {
    return line;
}
