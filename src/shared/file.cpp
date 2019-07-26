// July 26, 2019

#include "file.h"

File::File(const char * path, File::Mode mode) {
    file = fopen(path, mode == Read ? "r" : mode == Write ? "w" : "a");

    if (file == NULL) {
        ostringstream stream;
        stream << "Cannot open config file " << path;
        throw Exception(HERE, stream.str(), errno);
    }
}

File::~File() {
    fclose(file);
}

FILE * File::getPointer() {
    return file;
}
