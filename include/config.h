// July 26, 2019

#ifndef CONFIG_H
#define CONFIG_H

class ConfigException {
public:
    ConfigException(int line, const string & reason) : line(line), reason(reason) { }
    int getLine();

    friend ostream & operator << (ostream & os, const ConfigException & e);

private:
    int line;
    string reason;
};

class SyntaxException : public ConfigException {
public:
    SyntaxException(int line) : ConfigException(line, "Invalid syntax") { }
};

class EmptyException : public ConfigException {
public:
    EmptyException() : ConfigException(0, "Configuration is empty") { }
};

class SemanticException : public ConfigException {
public:
    SemanticException(int line, const string & reason) : ConfigException(line, reason) { }
};


class Config {
public:
    Config(const char * path);
    ~Config();

    yaml_node_t * root();
    void addIssue(const SemanticException & e);
    void logIssues();
    yaml_node_t * operator [] (int index);

private:
    string path;
    yaml_document_t document;
    vector<SemanticException> issues;
};

class Parser {
public:
    Parser(File & file);
    ~Parser();

    void load(yaml_document_t & document);

private:
    yaml_parser_t parser;
};

class Scalar {
public:
    Scalar(yaml_node_t * node);
    operator char * ();
    operator string ();
    operator long ();

private:
    yaml_node_t * node;
};

class Mapping {
public:
    Mapping(yaml_node_t * node);
    yaml_node_pair_t * begin();
    yaml_node_pair_t * end();

private:
    yaml_node_t * node;
};

class Sequence {
public:
    Sequence(yaml_node_t * node);
    yaml_node_item_t * begin();
    yaml_node_item_t * end();

private:
    yaml_node_t * node;
};

#endif
