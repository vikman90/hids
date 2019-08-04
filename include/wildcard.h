// August 4, 2019

#ifndef WILDCARD_H
#define WILDCARD_H

class Wildcard {
public:
    Wildcard(const string & pattern);
    ~Wildcard();

    char ** begin();
    char ** end();

private:
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

class NoMatch : Exception {
public:
    NoMatch(const string & func, const string & file, int line) : Exception(func, file, line, "No found matches") { }
};

#endif
