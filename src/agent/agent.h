// July 26, 2019

#ifndef AGENT_H
#define AGENT_H

#include "../shared/shared.h"

class AgentConfig : Config {
public:
    void read(const char * path);
};

#endif
