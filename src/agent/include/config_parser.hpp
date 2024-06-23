// June 29, 2019
// June 23, 2024

#pragma once

#include <string>
#include <yaml.h>

class ConfigParser {
public:
    ConfigParser(const std::string& configFilePath) : configFilePath(configFilePath) { }
    int parse();

private:
    int loadConfig();
    void parseRoot();
    void parseLogcollector(yaml_node_t * node);
    void parseFIM(yaml_node_t * node);

    const std::string configFilePath;
    yaml_document_t document;
};
