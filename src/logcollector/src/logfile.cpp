// June 29, 2019
// June 23, 2024

#include "../include/logfile.hpp"
#include <cstring>
#include <cerrno>
#include <logger.hpp>


#include <fstream>

bool Logfile::open() {
    if (error) {
        return false;
    }

    fp = fopen(path.c_str(), "r");

    if (fp == nullptr) {
        if (!warn) {
            Logger::warn("Cannot open log file '{}': {}", path, strerror(errno));
            warn = true;
        }

        offset = 0;
        return false;
    }

    struct stat buf;

    if (fstat(fileno(fp), &buf) == -1) {
        if (!warn) {
            Logger::error("Cannot stat log file '{}': {}", path, strerror(errno));
            warn = true;
        }

        offset = 0;
        fclose(fp);
        return false;
    }

    if (!S_ISREG(buf.st_mode)) {
        if (!warn) {
            Logger::warn("Cannot follow not regular file: {}", path);
            warn = true;
        }

        offset = 0;
        fclose(fp);
        return false;
    }

    if (inode == 0) {
        if (fseek(fp, 0, SEEK_END) == -1) {
            Logger::error("Cannot go to end of file '{}': {}", path, strerror(errno));
            error = 1;
            fclose(fp);
            return false;
        }

        inode = buf.st_ino;
    } else if (buf.st_ino != inode) {
        Logger::info("Log file inode has changed: {}", path);
        inode = buf.st_ino;
    } else if (buf.st_size < offset) {
        Logger::info("Log file has been shrunk: {}", path);
        offset = 0;
    } else if (fseek(fp, offset, SEEK_SET) == -1) {
        Logger::error("Cannot set file offset '{}': {}", path, strerror(errno));
        error = 1;
        fclose(fp);
        return false;
    }

    warn = false;
    return true;
}


void Logfile::close() {
    offset = ftell(fp);
    fclose(fp);
}
