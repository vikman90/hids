/* June 23, 2019 */

#ifndef HIDS_H
#define HIDS_H

#define _XOPEN_SOURCE 700
#define _BSD_SOURCE
#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <fcntl.h>
#include <libgen.h>
#include <search.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <sys/select.h>
#include <sys/inotify.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <yaml.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <cjson/cJSON.h>

#define print_info(msg, ...) fprintf(stderr, "\e[94m%s: \e[92mINFO:\e[39m " msg "\n", cur_module->name, ##__VA_ARGS__)
#define print_warn(msg, ...) fprintf(stderr, "\e[94m%s: \e[93mWARN:\e[39m " msg "\n", cur_module->name, ##__VA_ARGS__)
#define print_error(msg, ...) fprintf(stderr, "\e[94m%s: \e[91mERROR:\e[39m " msg "\n", cur_module->name, ##__VA_ARGS__)
#define print_critical(msg, ...) fprintf(stderr, "\e[94m%s: \e[91mCRITICAL:\e[39m " msg "\n", cur_module->name, ##__VA_ARGS__)

#define loop_path(x) (x[0] == '.' && (x[1] == '\0' || (x[1] == '.' && x[2] == '\0')))
#define time_diff(x, y) ((x.tv_sec - y.tv_sec) + (x.tv_nsec - y.tv_nsec) / 1e9)

#define BUFFER_SIZE 4096
#define CONFIG_FILE "etc/agent.yaml"
#define SHA256_LEN 65

typedef struct module_t {
    char * name;
    int (* main)();
    pid_t pid;
    FILE * stdin;
    FILE * stdout;
} module_t;

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

typedef struct {
    char * path;
    unsigned int warn:1;
} fim_item_t;

typedef struct {
    fim_item_t * items;
    unsigned length;
    char ** files;
    unsigned nfiles;
    long size_limit;
    unsigned long max_files;
    int inotify_fd;
    char ** inotify_wd_array;
    int inotify_wd_top;
    unsigned int follow_links:1;
    unsigned int real_time:1;
} fim_t;

extern logcollector_t logcollector;
extern fim_t fim;

extern module_t modules[];
extern module_t * cur_module;
extern const char * argv0;

int fim_main();
int logcollector_main();
int sca_main();
int openscap_main();
int osquery_main();
int rootcheck_main();
int ciscat_main();
int inventory_main();
int aws_main();
int azure_main();
int docker_main();
int command_main();

void report();
void handler(int signum);
void set_handler(int signum, void (*handler)(int));
void kill_modules();
void spawn(module_t * m);
void dispatch();
void dispatch_stdin(time_t timeout_sec);
void watchdog(module_t * m);
__attribute__((noreturn)) void critical(const char * s);
void cloexec(int fd);
void nonblock(int fd);
void set_name(const char * name);
void set_cwd();
int parse_config();
int file_sha256(int fd, char sum[SHA256_LEN]);

#endif
