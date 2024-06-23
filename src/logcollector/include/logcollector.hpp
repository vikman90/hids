// June 23, 2024

#pragma once

#include <module.hpp>
#include <logfile.hpp>
#include <vector>

class Logcollector : public Module<Logcollector> {
public:
    void addLogfile(const std::string &filename);

protected:
    void run() override;

private:
    friend class Module<Logcollector>;
    Logcollector() : Module("logcollector") { }
    void sendLog(const char * location, const char * log);

    std::vector<Logfile> files;
};
