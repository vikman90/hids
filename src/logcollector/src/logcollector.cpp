// June 29, 2019
// June 23, 2024

#include "../include/logcollector.hpp"
#include <logger.hpp>
#include <config.h>
#include <cstring>
#include <cjson/cJSON.h>

using namespace std;

void Logcollector::run() {
    if (files.empty()) {
        Logger::info("No logcollector items defined");
        return;
    }

    for (;;) {
        for (auto & logfile : files) {
            if (!logfile.open()) {
                continue;
            }

            char buffer[BUFFER_SIZE];

            while (fgets(buffer, BUFFER_SIZE, logfile.getFile())) {
                char * end = buffer + strlen(buffer) - 1;

                if (*end == '\n') {
                    *end = '\0';
                    sendLog(logfile.path.c_str(), buffer);
                } else {
                    Logger::warn("Discarding too long line from '{}'", logfile.path.c_str());
                    while (fgets(buffer, BUFFER_SIZE, logfile.getFile()) && buffer[strlen(buffer) - 1] != '\n');
                }
            }

            logfile.close();
        }

        io->popCommand();
    }
}

void Logcollector::addLogfile(const string &filename) {
    files.emplace_back(filename);
}

void Logcollector::sendLog(const char * location, const char * log) {
    cJSON * root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "location", location);
    cJSON_AddStringToObject(root, "log", log);

    char * payload = cJSON_PrintUnformatted(root);
    io->pushStateless(payload);

    free(payload);
    cJSON_Delete(root);
}
