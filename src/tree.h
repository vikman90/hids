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
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/random.h>
#include <sys/socket.h>
#include <sys/prctl.h>
#include "cJSON.h"

#define print_info(tag, msg, ...) fprintf(stderr, "\e[94m%s: \e[92mINFO:\e[39m " msg "\n", tag, ##__VA_ARGS__)
#define print_warn(tag, msg, ...) fprintf(stderr, "\e[94m%s: \e[93mWARN:\e[39m " msg "\n", tag, ##__VA_ARGS__)
#define print_error(tag, msg, ...) fprintf(stderr, "\e[94m%s: \e[91mERROR:\e[39m " msg "\n", tag, ##__VA_ARGS__)
#define print_critical(tag, msg, ...) fprintf(stderr, "\e[94m%s: \e[91mCRITICAL:\e[39m " msg "\n", tag, ##__VA_ARGS__)

#define BUFFER_SIZE 4096

typedef struct {
    size_t alloc;
    char * data;
    size_t length;
    size_t recv;
} buffer_t;

typedef struct module_t {
    char * name;
    int (* main)(struct module_t * module);
    pid_t pid;
    FILE * stdin;
    FILE * stdout;
    int sock;
    buffer_t stdout_buf;
} module_t;

extern module_t modules[];
extern module_t * cur_module;

int fim_main(module_t * module);
int logcollector_main(module_t * module);
int sca_main(module_t * module);
int openscap_main(module_t * module);
int osquery_main(module_t * module);
int rootcheck_main(module_t * module);
int ciscat_main(module_t * module);
int inventory_main(module_t * module);
int aws_main(module_t * module);
int azure_main(module_t * module);
int docker_main(module_t * module);
int command_main(module_t * module);

void report();
void handler(int signum);
void set_handler(int signum, void (*handler)(int));
void kill_modules();
void spawn(module_t * m);
void dispatch();
void dispatch_socket();
void watchdog(module_t * m);
__attribute__((noreturn)) void critical(const char * tag, const char * s);
void cloexec(int fd);
void nonblock(int fd);
void set_name(const char * name);
void write_msg(int fd, const char * data, size_t count);
ssize_t read_msg(int fd, buffer_t * buffer);
void buffer_clear(buffer_t * buffer);

#endif
