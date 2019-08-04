// July 26, 2019

#include <shared.h>
#include "logcollector.h"

Logcollector::Logcollector() : Module("logcollector") {
}

Logcollector::~Logcollector() {
    for (LogItem * item : items) {
        delete item;
    }
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
                            items.push_back(new LogItem(Scalar(config[item])));
                        }
                    }
                }
            }
        }
    } catch (EmptyException) {
    }
}

void Logcollector::run() {
    for (LogItem * item : items) {
        item->expand();
    }
}

LogItem::~LogItem() {
    for (File * file : files) {
        delete file;
    }
}

void LogItem::expand() {
    try {
        Wildcard wildcard(pattern);

        for (char * path : wildcard) {
            try {
                File * file = new File(path);

                if (S_ISREG(file->stat().st_mode)) {
                    Logger(HERE, Info)  << "Reading file " << path;
                    files.push_back(file);
                } else {
                    Logger(HERE, Warn) << path << " is not a file";
                    delete file;
                }
            } catch (Exception & e) {
                Logger(HERE, Error) << e;
            }
        }
    } catch (NoMatch) {
    } catch (Exception & e) {
        Logger(HERE, Error) << e;
    }
}
