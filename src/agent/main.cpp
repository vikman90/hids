// July 26, 2019

#include "agent.h"

int main() {

    try {
        Config config("etc/agent.yaml");

        Module * logc = new Logcollector;
        logc->load(config);
        delete logc;

    } catch (Exception & e) {
        cerr << e << endl;
    }

    return 0;
}
