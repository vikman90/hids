// August 4, 2019

#ifndef WILDCARD_H
#define WILDCARD_H

class Wildcard {
public:
    Wildcard(const string & pattern = "");
    ~Wildcard();

    void append(const string & pattern);
    bool find(const string & path);
    char ** begin();
    char ** end();

private:
    void glob(const string & pattern, int flags);

    glob_t globbuf;
};

class NoSpace : Exception {
public:
    NoSpace(const string & func, const string & file, int line) : Exception(func, file, line, "Out of memory") { }
};

class Aborted : Exception {
public:
    Aborted(const string & func, const string & file, int line) : Exception(func, file, line, "Read error") { }
};

#endif
