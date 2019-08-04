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

class Logcollector : public Module {
public:
    Logcollector();
    ~Logcollector();

    void load(Config & config);
    void run();

private:
    void addPattern(const char * pattern);
    void addFile(const char * path);
    list<File *>::iterator removeFile(list<File *>::iterator it);
    void checkFiles();

    vector<string> patterns;
    list<File *> files;
};

#endif
