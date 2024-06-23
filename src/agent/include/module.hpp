// June 22, 2024

#pragma once

#include <string>
#include <thread>

#include "io.hpp"

template <typename T>
class Module {
public:
    virtual ~Module() = default;
    void start() {
        thread = std::thread([&]() {
            this->run();
        });
    }
    void stop() {
       thread.join();
    }

    static T& getInstance() {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        static T instance;
        return instance;
    }

    void setIO(IModuleIO &io) {
        this->io = &io;
    }

    const std::string name;

protected:
    Module(const std::string &name) : name(name) { }

    virtual void run() = 0;

    IModuleIO *io;

private:
    Module(const Module&) = delete;
    Module& operator=(const Module&) = delete;

    std::thread thread;
};
