/* June 23, 2019 */

#ifndef TREE_H
#define TREE_H

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
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/random.h>
#include <sys/socket.h>
#include <sys/prctl.h>
#include <yaml.h>
#include "cJSON.h"

#define print_info(msg, ...) fprintf(stderr, "\e[94m%s: \e[92mINFO:\e[39m " msg "\n", cur_module->name, ##__VA_ARGS__)
#define print_warn(msg, ...) fprintf(stderr, "\e[94m%s: \e[93mWARN:\e[39m " msg "\n", cur_module->name, ##__VA_ARGS__)
#define print_error(msg, ...) fprintf(stderr, "\e[94m%s: \e[91mERROR:\e[39m " msg "\n", cur_module->name, ##__VA_ARGS__)
#define print_critical(msg, ...) fprintf(stderr, "\e[94m%s: \e[91mCRITICAL:\e[39m " msg "\n", cur_module->name, ##__VA_ARGS__)

#define BUFFER_SIZE 4096
#define CONFIG_FILE "etc/agent.yaml"

typedef struct module_t {
    char * name;
    int (* main)();
    pid_t pid;
    FILE * stdin;
    FILE * stdout;
    int sock;
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

extern logcollector_t logcollector;

extern module_t modules[];
extern module_t * cur_module;

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
void dispatch_socket();
void watchdog(module_t * m);
__attribute__((noreturn)) void critical(const char * s);
void cloexec(int fd);
void nonblock(int fd);
void set_name(const char * name);
void set_cwd(const char * argv0);
int parse_config();
int is_regular(const char * path);

#endif
