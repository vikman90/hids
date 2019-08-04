// August 4, 2019

#include <shared.h>

Wildcard::Wildcard(const string & pattern) {
    glob(pattern, 0);
}

Wildcard::~Wildcard() {
    globfree(&globbuf);
}

void Wildcard::append(const string & pattern) {
    glob(pattern, GLOB_APPEND);
}

bool Wildcard::find(const string & path) {
    for (size_t i = 0; i < globbuf.gl_pathc; ++i) {
        if (path == globbuf.gl_pathv[i]) {
            return true;
        }
    }

    return false;
}

char ** Wildcard::begin() {
    return globbuf.gl_pathv;
}
char ** Wildcard::end() {
    return globbuf.gl_pathv + globbuf.gl_pathc;
}

void Wildcard::glob(const string & pattern, int flags) {
    switch (::glob(pattern.c_str(), flags, nullptr, &globbuf)) {
    case 0:
        break;

    case GLOB_NOSPACE:
        throw NoSpace(HERE);

    case GLOB_ABORTED:
        throw Aborted(HERE);

    case GLOB_NOMATCH:
        break;
    }
}
