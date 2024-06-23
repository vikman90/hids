// June 22, 2024

#include <iostream>
#include <module.hpp>
#include <io.hpp>
#include <config.h>
#include <logger.hpp>

using namespace std;

class Dummy : public Module<Dummy> {
public:
    void addLogfile(const std::string &filename);

protected:
    void run() override {
        io->pushStateless("Hello World!");
    }

private:
    friend class Module<Dummy>;
    Dummy() : Module("dummy") { }
};

int main() {
    Logger::getInstance().setLogFile(LOG_PATH);
    BasicIO io;

    Dummy & dummy = Dummy::getInstance();
    dummy.setIO(io);

    dummy.start();
    Logger::info("Agent started successfully");
    dummy.stop();

    return 0;
}
