// July 26, 2019

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "shared.h"

#define HERE __func__, __FILE__, __LINE__

class Exception {
public:
    Exception(const string & func, const string & file, int lineno, const string & message, int _errno = 0);

    friend ostream & operator << (ostream & os, const Exception & e);

private:
    string func;
    string file;
    int lineno;
    string message;
    int _errno;
};

#endif
