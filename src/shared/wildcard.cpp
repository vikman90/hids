// August 4, 2019

#include <shared.h>

Wildcard::Wildcard(const string & pattern) {
    switch (glob(pattern.c_str(), 0, nullptr, &globbuf)) {
    case 0:
        break;

    case GLOB_NOSPACE:
        throw NoSpace(HERE);

    case GLOB_ABORTED:
        throw Aborted(HERE);

    case GLOB_NOMATCH:
        throw NoMatch(HERE);
    }
}

Wildcard::~Wildcard() {
    globfree(&globbuf);
}

char ** Wildcard::begin() {
    return globbuf.gl_pathv;
}
char ** Wildcard::end() {
    return globbuf.gl_pathv + globbuf.gl_pathc;
}
