// July 26, 2019

#include "exception.h"

Exception::Exception(const string & func, const string & file, int lineno, const string & message, int _errno) : func(func), file(file), lineno(lineno), message(message), _errno(_errno) { }

ostream & operator << (ostream & os, const Exception & e) {
#ifdef NDEBUG
    os << e.message;
#else
    os << "[" << e.file << ":" << e.lineno << "] at " << e.func << "(): " << e.message;
#endif

    if (e._errno) {
        os << ": " << strerror(e._errno) << " (" << e._errno << ")";
    }

    return os;
}
