// July 26, 2019

#include <shared.h>

File::File(const char * path, File::Mode mode) {
    file = fopen(path, mode == Read ? "r" : mode == Write ? "w" : "a");

    if (file == NULL) {
        ostringstream stream;
        stream << "Cannot open file \"" << path << "\"";
        throw Exception(HERE, stream.str(), errno);
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
