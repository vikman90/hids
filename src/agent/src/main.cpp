// June 22, 2024

#include <iostream>
#include <io.hpp>
#include <config.h>
#include <logger.hpp>
#include <logcollector.hpp>

using namespace std;

int main() {
    Logger::getInstance().setLogFile(LOG_PATH);
    BasicIO io;

    Logcollector & logcollector = Logcollector::getInstance();
    logcollector.setIO(io);

    logcollector.addLogfile("/var/log/secure");
    logcollector.addLogfile("/var/log/messages");
    logcollector.addLogfile("/var/log/auth.log");
    logcollector.addLogfile("/var/log/syslog");
    logcollector.addLogfile("/root/example.log");
    logcollector.addLogfile("/proc/self/fd/0");
    logcollector.addLogfile("/dev");

    logcollector.start();
    Logger::info("Agent started successfully");
    logcollector.stop();

    return 0;
}
