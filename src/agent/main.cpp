// July 26, 2019

#include "agent.h"

int main() {
    Module * logc = NULL;

    try {
        Config config("etc/agent.yaml");

        logc = new Logcollector;
        logc->load(config);
        logc->run();
    } catch (Exception & e) {
        cerr << e << endl;
    }

    delete logc;

    return 0;
}
