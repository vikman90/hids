// August 3, 2019

#include <shared.h>

Logger::Logger(const string & func, const string & file, int line, LogLevel level) : level(level) {
    if (level <= level_threshold) {
        struct tm tm;
        time_t now = time(NULL);
        localtime_r(&now, &tm);
        buffer << "\e[97m" << tm.tm_year + 1900 << "/" << tm.tm_mon + 1 << "/" << tm.tm_mday << " " << tm.tm_hour << ":" << tm.tm_min << ":" << tm.tm_sec << "\e[0m ";

        if (level_threshold == Debug) {
            buffer << "\e[90m" << file << ":" << line << " at " << func << "()\e[0m ";
        }

        switch (level) {
        case Error:
            buffer << "[\e[31mERROR\e[0m] ";
            break;
        case Warn:
            buffer << "[\e[33mWARN\e[0m] ";
            break;
        case Info:
            buffer << "[\e[32mINFO\e[0m] ";
            break;
        case Debug:
            buffer << "[\e[34mDEBUG\e[0m] ";
            break;
        }
    }
}

Logger::~Logger() {
    if (level <= level_threshold) {
        buffer << std::endl;
        std::cerr << buffer.str();
    }
}

void Logger::logLevel(LogLevel level) {
    level_threshold = level;
}

LogLevel Logger::level_threshold = Debug;
