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
    glob_t buf;
    ostringstream stream;

    switch (glob(pattern.c_str(), 0, NULL, &buf)) {
    case 0:
        break;

    case GLOB_NOSPACE:
        stream << "Cannot expand \"" << pattern << "\": out of memory";
        throw Exception(HERE, stream.str());

    case GLOB_ABORTED:
        stream << "Cannot expand \"" << pattern << "\": read error";
        throw Exception(HERE, stream.str());

    case GLOB_NOMATCH:
        return;
    }

    for (size_t i = 0; i < buf.gl_pathc; ++i) {
        try {
            File * file = new File(buf.gl_pathv[i]);

            if (S_ISREG(file->stat().st_mode)) {
                Logger(HERE, Info)  << "Reading file " << buf.gl_pathv[i];
                files.push_back(file);
            } else {
                Logger(HERE, Warn) << buf.gl_pathv[i] << " is not a file";
                delete file;
            }
        } catch (Exception & e) {
            Logger(HERE, Error) << e;
        }
    }

    globfree(&buf);
}
