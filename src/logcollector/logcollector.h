// July 26, 2019

#ifndef LOGCOLLECTOR_H
#define LOGCOLLECTOR_H

// class LogItem {
// public:
//     LogFile(const string & location);
//     ~LogFile();

//     void read();

// private:
//     string location;
//     long offset;
//     ino_t inode;
//     unsigned int error:1;
//     unsigned int warn:1;
// };

class LogItem {
public:
    LogItem(const char * pattern) : pattern(pattern) { }
    ~LogItem();

    void expand();

private:
    string pattern;
    vector<File *> files;
};

class Logcollector : public Module {
public:
    Logcollector();
    ~Logcollector();

    void load(Config & config);
    void run();

private:
    vector<LogItem *> items;
};

#endif
