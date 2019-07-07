/* June 29, 2019 */

#include "hids.h"

fim_t fim;

static void fim_item(fim_item_t * item);
static int fim_link(fim_item_t * item, const char * path);
static void fim_path(fim_item_t * item, const char * path);
static void fim_dir(fim_item_t * item, int fd, const char * path);
static void fim_file(int fd, const char * path, struct stat * statbuf);
static void fim_add(const char * path);
static void fim_free();

int fim_main() {
    if (fim.length == 0) {
        print_info("No FIM items defined");
        return EXIT_SUCCESS;
    }

    if (fim.size_limit == 0) {
        /* Default size limit: 1 GiB */
        fim.size_limit = 1073741824;
    }

    for (;;) {
        struct timespec tp0, tp1;

        hcreate(1000000);
        clock_gettime(CLOCK_MONOTONIC, &tp0);

        for (unsigned i = 0; i < fim.length; i++) {
            fim_item(fim.items + i);
        }

        clock_gettime(CLOCK_MONOTONIC, &tp1);
        print_info("FIM scan ended. Files: %u. Time: %f seconds.", fim.nfiles, time_diff(tp1, tp0));
        fim_free();
        hdestroy();
        dispatch_stdin(60);
    }

    return EXIT_SUCCESS;
}

void fim_item(fim_item_t * item) {
    if (fim_link(item, item->path) == -1) {
        if (!item->warn) {
            print_warn("Cannot read '%s': %s", item->path, strerror(errno));
            item->warn = 1;
        }
    }
}

int fim_link(fim_item_t * item, const char * path) {
    struct stat buf;

    if (lstat(path, &buf) == -1) {
        return -1;
    } else if ((buf.st_mode & S_IFMT) == S_IFLNK) {
        if (item->follow_links) {
            char real[PATH_MAX];

            if (realpath(path, real)) {
                fim_path(item, real);
            } else {
                return -1;
            }
        }
    } else {
        fim_path(item, path);
    }

    return 0;
}

void fim_path(fim_item_t * item, const char * path) {
    ENTRY entry = { .key = (char *)path, .data = NULL };

    if (hsearch(entry, FIND)) {
        print_info("Duplicate item: '%s'", path);
        return;
    }

    fim_add(path);

    /* O_NONBLOCK prevents open() from blocking on FIFO files */
    int fd = open(path, O_RDONLY | O_NONBLOCK);

    if (fd == -1) {
        print_warn("Cannot open '%s': %s", path, strerror(errno));
        return;
    }

    struct stat buf;

    if (fstat(fd, &buf) == -1) {
        print_error("Cannot stat '%s': %s", path, strerror(errno));
        return;
    }

    switch (buf.st_mode & S_IFMT) {
    case S_IFDIR:
        fim_dir(item, fd, path);
        break;

    case S_IFREG:
        fim_file(fd, path, &buf);
        break;

    default:
        print_info("Ignoring '%s': not a regular file", path);
        close(fd);
    }
}

void fim_dir(fim_item_t * item, int fd, const char * path) {
    DIR * dir = fdopendir(fd);

    if (dir == NULL) {
        print_error("Cannot open directory '%s': %s", path, strerror(errno));
    } else {
        struct dirent * entry;

        while ((entry = readdir(dir))) {
            if (!loop_path(entry->d_name)) {
                char lpath[PATH_MAX];
                snprintf(lpath, PATH_MAX, "%s%s%s", path, (path[strlen(path) - 1] == '/') ? "" : "/", entry->d_name);
                fim_link(item, lpath);
            }
        }
    }

    closedir(dir);
}

void fim_file(int fd, const char * path, struct stat * statbuf) {
    char sha256[SHA256_LEN];

    cJSON * jroot = cJSON_CreateObject();
    cJSON * jstat = cJSON_CreateObject();

    cJSON_AddStringToObject(jroot, "path", path);
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
        if (statbuf->st_size > fim.size_limit) {
            print_info("Ignoring '%s' SHA256: file exceeds size limit", path);
        } else if (file_sha256(fd, sha256) == 0) {
            cJSON_AddStringToObject(jroot, "sha256", sha256);
        } else {
            print_warn("Cannot calculate SHA256 sum of '%s'", path);
        }
    } else {
        cJSON_AddStringToObject(jroot, "sha256", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    }

    char * payload = cJSON_PrintUnformatted(jroot);
    printf("%s\n", payload);

    free(payload);
    cJSON_Delete(jroot);
    close(fd);
}

void fim_add(const char * path) {
    unsigned last = fim.nfiles++;
    fim.files = realloc(fim.files, fim.nfiles * sizeof(char *));

    if (fim.files == NULL) {
        critical("Cannot allocate memory for files");
    }

    fim.files[last] = strdup(path);

    ENTRY entry = { .key = fim.files[last], .data = NULL };
    hsearch(entry, ENTER);
}

void fim_free() {
    for (long i = 0; i < fim.nfiles; i++) {
        free(fim.files[i]);
    }

    free(fim.files);
    fim.files = NULL;
    fim.nfiles = 0;
}
