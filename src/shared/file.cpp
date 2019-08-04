// July 26, 2019

#include <shared.h>

File::File(const char * path, File::Mode mode) : path(path) {
    file = fopen(path, mode == Read ? "r" : mode == Write ? "w" : "a");

    if (file == NULL) {
        ostringstream stream;
        stream << "Cannot open file \"" << path << "\"";
        throw Exception(HERE, stream.str(), errno);
    }

    if (!S_ISREG(stat().st_mode)) {
        ostringstream stream;
        fclose(file);
        stream << "Path \"" << path << "\" is not a regular regular file";
        throw Exception(HERE, stream.str());
    }
}

File::~File() {
    fclose(file);
}

FILE * File::getPointer() {
    return file;
}

struct stat File::stat() const {
    struct stat buf;

    if (fstat(fileno(file), &buf) == -1) {
        throw Exception(HERE, "Cannot stat file");
    }

    return buf;
}

const string & File::getPath() const {
    return path;
}
