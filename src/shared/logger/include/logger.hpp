// June 23, 2024

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <mutex>
#include <format> // Requires C++20
#include <chrono>
#include <ctime>

class Logger {
public:
    enum class Level { Info, Warn, Error };

    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    template<typename... Args>
    static void info(const std::string& format, Args... args) {
        getInstance().log(Level::Info, format, args...);
    }

    template<typename... Args>
    static void warn(const std::string& format, Args... args) {
        getInstance().log(Level::Warn, format, args...);
    }

    template<typename... Args>
    static void error(const std::string& format, Args... args) {
        getInstance().log(Level::Error, format, args...);
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void setLogFile(const std::string& filename) {
        std::lock_guard<std::mutex> lock(mtx);
        logFile.open(filename, std::ios::out | std::ios::app);
    }

private:
    Logger() = default;

    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    std::ofstream logFile;
    std::mutex mtx;

    template<typename... Args>
    void log(Level level, const std::string& format, Args... args) {
        std::lock_guard<std::mutex> lock(mtx);
        std::string levelStr;

        switch (level) {
            case Level::Info: levelStr = " \e[92m[INFO]\e[39m "; break;
            case Level::Warn: levelStr = " \e[93m[WARN]\e[39m "; break;
            case Level::Error: levelStr = " \e[91m[ERROR]\e[39m "; break;
        }

        std::string message = std::vformat(format, std::make_format_args(args...));
        std::ostringstream ss;
        ss << getCurrentTime() << levelStr << message << std::endl;

        if (logFile.is_open()) {
            logFile << ss.str();
        }

        std::cerr << ss.str();
    }

    static std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        char buffer[20];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now_time));
        return std::string(buffer);
    }
};
