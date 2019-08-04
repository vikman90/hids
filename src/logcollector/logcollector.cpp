// July 26, 2019

#include <shared.h>
#include "logcollector.h"

Logcollector::Logcollector() : Module("logcollector") {
}

Logcollector::~Logcollector() {
    for (File * file : files) {
        delete file;
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
                                        addPattern(pattern);
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
    checkFiles();
}

void Logcollector::addPattern(const char * pattern) {
    for (string & p : patterns) {
        if (p == pattern) {
            debugLog << "Duplicate log entry: " << pattern;
            return;
        }
    }

    patterns.push_back(pattern);
}

void Logcollector::checkFiles() {
    Wildcard wildcard;

    for (string pattern : patterns) {
        try {
            wildcard.append(pattern);
        } catch (Exception & e) {
            errorLog << e;
        }
    }

    for (list<File *>::iterator it = files.begin(); it != files.end();) {
        if (!wildcard.find((*it)->getPath())) {
            it = removeFile(it);
        } else {
            ++it;
        }
    }

    for (char * path : wildcard) {
        addFile(path);
    }
}

void Logcollector::addFile(const char * path) {
    for (list<File *>::iterator it = files.begin(); it != files.end(); ++it) {
        if ((*it)->getPath() == path) {
            return;
        }
    }

    try {
        files.push_back(new File(path));
        infoLog  << "Reading file: " << path;
    } catch (Exception & e) {
        warnLog << e;
    }
}

list<File *>::iterator Logcollector::removeFile(list<File *>::iterator it) {
    infoLog << "Closing file: " << (*it)->getPath();
    delete * it;
    return files.erase(it);
}
