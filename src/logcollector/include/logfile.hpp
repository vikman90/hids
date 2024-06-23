// June 23, 2019
// June 23, 2024

#pragma once

#include <string>
#include <stdio.h>
#include <sys/stat.h>

class Logfile {
public:
    Logfile(const std::string & path) : path(path) { }
    bool open();
    void close();

    inline FILE * getFile() const { return fp; }

    const std::string path;

private:
    FILE * fp;
    long offset;
    ino_t inode;
    bool error;
    bool warn;
};
