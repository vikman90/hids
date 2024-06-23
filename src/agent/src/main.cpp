// June 22, 2024

#include <iostream>
#include <io.hpp>
#include <config.h>
#include <logger.hpp>
#include <logcollector.hpp>
#include <config_parser.hpp>

using namespace std;

int main() {
    Logger::getInstance().setLogFile(LOG_PATH);
    BasicIO io;

    Logcollector & logcollector = Logcollector::getInstance();
    logcollector.setIO(io);

    ConfigParser config(CONFIG_PATH);
    config.parse();

    logcollector.start();
    Logger::info("Agent started successfully");
    logcollector.stop();

    return 0;
}
