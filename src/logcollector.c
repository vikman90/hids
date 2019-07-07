/* June 29, 2019 */

#include "hids.h"

logcollector_t logcollector;

static FILE * open_logfile(logfile_t * file);

int logcollector_main() {
    if (logcollector.length == 0) {
        print_info("No logcollector items defined");
        return EXIT_SUCCESS;
    }

    for (;;) {
        for (unsigned i = 0; i < logcollector.length; i++) {
            logfile_t * logfile = logcollector.logfiles + i;
            FILE * fp = open_logfile(logfile);

            if (fp == NULL) {
                continue;
            }

            char buffer[BUFFER_SIZE];

            while (fgets(buffer, BUFFER_SIZE, fp)) {
                char * end = buffer + strlen(buffer) - 1;

                if (*end == '\n') {
                    *end = '\0';
                    cJSON * root = cJSON_CreateObject();
                    cJSON_AddStringToObject(root, "location", logfile->path);
                    cJSON_AddStringToObject(root, "log", buffer);
                    char * payload = cJSON_PrintUnformatted(root);
                    printf("%s\n", payload);
                    free(payload);
                    cJSON_Delete(root);

                } else {
                    print_warn("Discarding too long line from '%s'", logfile->path);
                    while (fgets(buffer, BUFFER_SIZE, fp) && buffer[strlen(buffer) - 1] != '\n');
                }
            }

            logfile->offset = ftell(fp);
            fclose(fp);
        }

        dispatch_stdin(1);
    }

    return EXIT_SUCCESS;
}

FILE * open_logfile(logfile_t * file) {
    if (file->error) {
        return NULL;
    }

    FILE * fp = fopen(file->path, "r");

    if (fp == NULL) {
        if (!file->warn) {
            print_warn("Cannot open log file '%s': %s", file->path, strerror(errno));
            file->warn = 1;
        }

        file->offset = 0;
        return NULL;
    }

    struct stat buf;

    if (fstat(fileno(fp), &buf) == -1) {
        if (!file->warn) {
            print_error("Cannot stat log file '%s': %s", file->path, strerror(errno));
            file->warn = 1;
        }

        file->offset = 0;
        fclose(fp);
        return NULL;
    }

    if (!S_ISREG(buf.st_mode)) {
        if (!file->warn) {
            print_warn("Cannot follow not regular file: %s", file->path);
            file->warn = 1;
        }

        file->offset = 0;
        fclose(fp);
        return NULL;
    }

    if (file->inode == 0) {
        if (fseek(fp, 0, SEEK_END) == -1) {
            print_error("Cannot go to end of file '%s': %s", file->path, strerror(errno));
            file->error = 1;
            fclose(fp);
            return NULL;
        }

        file->inode = buf.st_ino;
    } else if (buf.st_ino != file->inode) {
        print_info("Log file inode has changed: %s", file->path);
        file->inode = buf.st_ino;
    } else if (buf.st_size < file->offset) {
        print_info("Log file has been shrunk: %s", file->path);
        file->offset = 0;
    } else if (fseek(fp, file->offset, SEEK_SET) == -1) {
        print_error("Cannot set file offset '%s': %s", file->path, strerror(errno));
        file->error = 1;
        fclose(fp);
        return NULL;
    }

    file->warn = 0;
    return fp;
}
