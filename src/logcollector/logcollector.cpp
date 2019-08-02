// July 26, 2019

#include <shared.h>
#include "logcollector.h"

Logcollector::Logcollector() : Module("logcollector") {
}

Logcollector::~Logcollector() {
}

void Logcollector::load(Config & config) {
    try {
        Mapping components(config.root());

        for (yaml_node_pair_t pair : components) { 
            string key = Scalar(config[pair.key]);

            if (key == "logcollector") {
                Mapping options(config[pair.value]);

                for (yaml_node_pair_t pair : options) { 
                    string key = Scalar(config[pair.key]);

                    if (key == "files") {
                        Sequence files(config[pair.value]);

                        for (yaml_node_item_t item : files) {
                            cout << "New file: " << (char *)Scalar(config[item]) << endl;
                        }
                    }
                }
            }
        }
    } catch (EmptyException) {
    }
}

void Logcollector::run() {
}
