// June 22, 2024

#include <iostream>
#include <module.hpp>
#include <io.hpp>

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
    BasicIO io;

    Dummy & dummy = Dummy::getInstance();
    dummy.setIO(io);

    dummy.start();
    dummy.stop();

    return 0;
}
