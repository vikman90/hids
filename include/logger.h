// July 26, 2019

#ifndef LOGGER_H
#define LOGGER_H

enum LogLevel { Error, Warn, Info, Debug };

class Logger {
public:
    Logger(const string & func, const string & file, int line, LogLevel level);
    ~Logger();

    template <typename T> Logger & operator << (T const & value) {
        buffer << value;
        return *this;
    }

    static void logLevel(LogLevel level);

private:
    LogLevel level;
    std::ostringstream buffer;
    static LogLevel level_threshold;
};

#endif
