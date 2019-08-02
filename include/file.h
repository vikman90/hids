// July 26, 2019

#ifndef FILE_H
#define FILE_H

class File {
public:
    enum Mode { Read, Write, Append };

    File(const char * path, Mode mode = Read);
    ~File();

    FILE * getPointer();

private:
    FILE * file;
};

#endif