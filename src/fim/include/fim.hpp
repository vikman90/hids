// June 23, 2024

#pragma once

#include <module.hpp>
#include "fim_item.hpp"
#include "realtime.hpp"
#include <config.h>
#include <vector>
#include <sys/stat.h>

class FIM : public Module<FIM> {
public:
    void addItem(const std::string &filename);

    long size_limit;
    unsigned long max_files;
    bool follow_links;
    bool real_time;

protected:
    void run() override;

private:
    friend class Module<FIM>;

    FIM() : Module("fim"), size_limit(FIM_SIZE_LIMIT) { }

    void setup();
    void checkItem(FIMItem & item);
    int checkLink(const std::string & path);
    void checkPath(const std::string & path);
    void checkDirectory(int fd, const std::string & path);
    void checkFile(int fd, const std::string & path, const struct stat * statbuf);
    void addFile(const std::string & path);
    void resetFiles();

    std::vector<FIMItem> items;
    std::vector<std::string> files;
    Notify notify;
};
