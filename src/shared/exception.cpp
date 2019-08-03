// July 26, 2019

#include <shared.h>

bool Exception::trace;

Exception::Exception(const string & func, const string & file, int lineno, const string & message, int _errno) : func(func), file(file), lineno(lineno), message(message), _errno(_errno) { }

ostream & operator << (ostream & os, const Exception & e) {
    if (Exception::trace) {
        os << "[" << e.file << ":" << e.lineno << "] at " << e.func << "(): " << e.message;
    } else {
        os << e.message;
    }

    if (e._errno) {
        os << ": " << strerror(e._errno) << " (" << e._errno << ")";
    }

    return os;
}
