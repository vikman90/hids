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

#define errorLog Logger(HERE, Error)
#define warnLog Logger(HERE, Warn)
#define infoLog Logger(HERE, Info)
#define debugLog Logger(HERE, Debug)

#endif
