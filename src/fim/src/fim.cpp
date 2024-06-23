// June 29, 2019
// June 23, 2024

#include "../include/fim.hpp"
#include <logger.hpp>
#include <search.h> // hcreate, hdestroy, hsearch
#include <cerrno>
#include <cstring>
#include <ctime>
#include <climits>
#include <fcntl.h>
#include <unistd.h> // close
#include <sys/types.h> // opendir
#include <dirent.h> // opendir
#include <pwd.h> // getpwnam
#include <grp.h> // getgrgid
#include <cjson/cJSON.h>
#include <hash_op.h>
#include <time_op.h>
#include <file_op.h>

using namespace std;

void FIM::run() {
    if (items.empty()) {
        Logger::info("No FIM items defined");
        return;
    }

    setup();

    for (;;) {
        struct timespec tp0, tp1;

        clock_gettime(CLOCK_MONOTONIC, &tp0);

        for (auto & item: items) {
            checkItem(item);
        }

        clock_gettime(CLOCK_MONOTONIC, &tp1);
        Logger::info("FIM scan ended. Time: {} seconds.", time_diff(tp1, tp0));

        if (follow_links) {
            resetFiles();
        }

        if (real_time) {
            Logger::info("Watching files");

            for (;;) {
                notify.dispatchRealtime([&](const string & path) {
                    checkLink(path);
                });

                io->popCommand();
            }
        } else {
            io->popCommand(600);
        }
    }
}

void FIM::addItem(const string &filename) {
    items.emplace_back(filename);
}

void FIM::setup() {
    if (size_limit == 0) {
        /* Default size limit: 1 GiB */
        size_limit = FIM_SIZE_LIMIT;
    }

    if (max_files == 0) {
        max_files = 100000;
    }

    if (real_time) {
        notify.setup();
    }

    if (follow_links) {
        hcreate(max_files);
    }
}

void FIM::checkItem(FIMItem & item) {
    if (checkLink(item.path) == -1) {
        if (!item.warn) {
            Logger::warn("Cannot read '{}': {}", item.path, strerror(errno));
            item.warn = true;
        }
    }
}

int FIM::checkLink(const string & path) {
    struct stat buf;

    if (lstat(path.c_str(), &buf) == -1) {
        return -1;
    } else if ((buf.st_mode & S_IFMT) == S_IFLNK) {
        if (follow_links) {
            char real[PATH_MAX];

            if (realpath(path.c_str(), real)) {
                checkPath(real);
            } else {
                return -1;
            }
        }
    } else {
        checkPath(path);
    }

    return 0;
}

void FIM::checkPath(const string & path) {
    if (follow_links) {
        ENTRY entry = { .key = const_cast<char *>(path.c_str()), .data = nullptr };

        if (hsearch(entry, FIND)) {
            Logger::info("Duplicate item: '{}'", path);
            return;
        }
    }

    if (follow_links) {
        addFile(path);
    }

    /* O_NONBLOCK prevents open() from blocking on FIFO files */
    int fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);

    if (fd == -1) {
        Logger::warn("Cannot open '{}': {}", path, strerror(errno));
        return;
    }

    struct stat buf;

    if (fstat(fd, &buf) == -1) {
        Logger::error("Cannot stat '{}': {}", path, strerror(errno));
        return;
    }

    switch (buf.st_mode & S_IFMT) {
    case S_IFDIR:
        checkDirectory(fd, path);
        break;

    case S_IFREG:
        checkFile(fd, path, &buf);
        break;

    default:
        Logger::info("Ignoring '{}': not a regular file", path);
        close(fd);
    }
}

void FIM::checkDirectory(int fd, const string & path) {
    DIR * dir = fdopendir(fd);

    if (dir == nullptr) {
        Logger::error("Cannot open directory '{}': {}", path, strerror(errno));
    } else {
        if (real_time) {
            notify.setWatch(path);
        }

        struct dirent * entry;

        while ((entry = readdir(dir))) {
            if (!loop_path(entry->d_name)) {
                char lpath[PATH_MAX];
                snprintf(lpath, PATH_MAX, "%s%s%s", path.c_str(), (path[path.length() - 1] == '/') ? "" : "/", entry->d_name);
                checkLink(lpath);
                sched_yield();
            }
        }
    }

    closedir(dir);
}

void FIM::checkFile(int fd, const std::string & path, const struct stat * statbuf) {
    char sha256[SHA256_LEN];

    cJSON * jroot = cJSON_CreateObject();
    cJSON * jstat = cJSON_CreateObject();

    cJSON_AddStringToObject(jroot, "path", path.c_str());
    cJSON_AddItemToObject(jroot, "stat", jstat);

    cJSON_AddNumberToObject(jstat, "inode", statbuf->st_ino);
    cJSON_AddNumberToObject(jstat, "links", statbuf->st_nlink);
    cJSON_AddNumberToObject(jstat, "uid", statbuf->st_uid);
    cJSON_AddNumberToObject(jstat, "gid", statbuf->st_gid);
    cJSON_AddNumberToObject(jstat, "size", statbuf->st_size);
    cJSON_AddNumberToObject(jstat, "access_time", statbuf->st_atime);
    cJSON_AddNumberToObject(jstat, "modification_time", statbuf->st_mtime);
    cJSON_AddNumberToObject(jstat, "creation_time", statbuf->st_ctime);

    /* Owner and group */

    struct passwd * owner = getpwuid(statbuf->st_uid);

    if (owner) {
        cJSON_AddStringToObject(jroot, "owner", owner->pw_name);
    }

    struct group * group = getgrgid(statbuf->st_gid);

    if (group) {
        cJSON_AddStringToObject(jroot, "group", group->gr_name);
    }

    /* SHA256 */

    if (statbuf->st_size > 0) {
        if (statbuf->st_size > size_limit) {
            Logger::info("Ignoring '{}' SHA256: file exceeds size limit", path);
        } else if (file_sha256(fd, sha256) == 0) {
            cJSON_AddStringToObject(jroot, "sha256", sha256);
        } else {
            Logger::warn("Cannot calculate SHA256 sum of '{}'", path);
        }
    } else {
        cJSON_AddStringToObject(jroot, "sha256", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    }

    char * payload = cJSON_PrintUnformatted(jroot);
    io->pushStateful(payload);

    free(payload);
    cJSON_Delete(jroot);
    close(fd);
}

void FIM::addFile(const string & path) {
    files.push_back(path);

    ENTRY entry = { .key = const_cast<char *>(files.back().c_str()), .data = nullptr };

    if (hsearch(entry, ENTER) == nullptr) {
        Logger::error("Scan exceeded the file limit ({})", max_files);
    }
}

void FIM::resetFiles() {
    files.clear();
    hdestroy();
    hcreate(max_files);
}
