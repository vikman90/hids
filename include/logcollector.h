/* July 23, 2019 */

#ifndef LOGCOLLECTOR_H
#define LOGCOLLECTOR_H

typedef struct {
    char * path;
    long offset;
    ino_t inode;
    unsigned int error:1;
    unsigned int warn:1;
} logfile_t;

typedef struct {
    logfile_t * logfiles;
    unsigned length;
} logcollector_t;

extern logcollector_t logcollector;

int logcollector_main();

#endif
