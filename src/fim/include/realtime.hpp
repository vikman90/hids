// June 23, 2024

#pragma once

#include <map>
#include <string>
#include <functional>

class Notify {
public:
    void setup();

    void setWatch(const std::string & path);
    void dispatchRealtime(std::function<void(const std::string &)> callback);

private:
    int inotify_fd;
    std::map<int, std::string> inotify_watches;
};
