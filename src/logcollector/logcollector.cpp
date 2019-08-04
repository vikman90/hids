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
    yaml_node_t * root;

    try {
        root = config.root();
    } catch (EmptyException) {
        debugLog << "Configuration is empty";
        return;
    }

    try {
        Mapping components(root);

        for (yaml_node_pair_t & pair : components) {
            string key = Scalar(config[pair.key]);

            if (key == "logcollector") {
                try {
                    Mapping options(config[pair.value]);

                    for (yaml_node_pair_t & pair : options) {
                        string key = Scalar(config[pair.key]);

                        if (key == "files") {
                            try {
                                Sequence files(config[pair.value]);

                                for (yaml_node_item_t & item : files) {
                                    try {
                                        char * pattern = Scalar(config[item]);
                                        items.push_back(new LogItem(pattern));
                                    } catch (SemanticException & e) {
                                        config.addIssue(e);
                                    }
                                }
                            } catch (SemanticException & e) {
                                config.addIssue(e);
                            }
                        }
                    }
                } catch (SemanticException & e) {
                    config.addIssue(e);
                }
            }
        }

    } catch (SemanticException & e) {
        config.addIssue(e);
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
                    infoLog  << "Reading file " << path;
                    files.push_back(file);
                } else {
                    warnLog << path << " is not a file";
                    delete file;
                }
            } catch (Exception & e) {
                errorLog << e;
            }
        }
    } catch (NoMatch) {
        debugLog << "Pattern \"" << pattern << "\" matched no files";
    } catch (Exception & e) {
        errorLog << e;
    }
}
