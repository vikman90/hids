// July 26, 2019

#ifndef CONFIG_H
#define CONFIG_H

#include "shared.h"

class File;

class Config {
protected:
    ~Config();

    void read(const char * path);

    yaml_document_t document;
};

class Parser {
public:
    Parser(File & file);
    ~Parser();

    void load(yaml_document_t & document);

    class Exception {
    public:
        Exception(int line);
        int getLine();

    private:
        int line;
    };

private:
    yaml_parser_t parser;
};

#endif
