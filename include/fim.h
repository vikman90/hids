/* July 23, 2019 */

#ifndef FIM_H
#define FIM_H

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

extern fim_t fim;

int fim_main();

#endif
