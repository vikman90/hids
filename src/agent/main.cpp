// July 26, 2019

#include "agent.h"

int main() {
    AgentConfig config;

    try {
        config.read("etc/agent.yaml");
    } catch (Exception & e) {
        cerr << e << endl;
    }

    return 0;
}

void AgentConfig::read(const char * path) {
    Config::read(path);
}
