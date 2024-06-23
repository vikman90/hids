// June 22, 2024

#pragma once

#include <string>

struct IModuleIO {
    virtual ~IModuleIO() = default;

    virtual void pushStateful(const std::string & data) = 0;
    virtual void pushStateless(const std::string & data) = 0;
    virtual std::string popCommand(int timeout_sec = 1) = 0;
};

struct BasicIO : IModuleIO {
    void pushStateful(const std::string & data);
    void pushStateless(const std::string & data);
    std::string popCommand(int timeout_sec = 1);
};
