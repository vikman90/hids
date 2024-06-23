// June 29, 2019
// June 23, 2024

#include "../include/fim.hpp"
#include <logger.hpp>
#include <sys/inotify.h>
#include <climits>
#include <cerrno>
#include <cstring>
#include <unistd.h>

using namespace std;

void Notify::setup() {
    inotify_fd = inotify_init1(IN_NONBLOCK);

    if (inotify_fd == -1) {
        Logger::error("Cannot create an inotify instance");
        abort();
    }
}

void Notify::setWatch(const string & path) {
    int wd = inotify_add_watch(inotify_fd, path.c_str(), IN_CLOSE_WRITE | IN_CREATE | IN_DELETE);

    if (wd == -1) {
        Logger::error("Cannot watch {}: {}", path, strerror(errno));
    } else {
        inotify_watches[wd] = path;
    }
}

void Notify::dispatchRealtime(std::function<void(const std::string &)> callback) {
    char buffer[sizeof(struct inotify_event) + NAME_MAX + 1];
    struct inotify_event * event;
    ssize_t count;

    while ((count = read(inotify_fd, buffer, sizeof(buffer))) > 0) {
        for (unsigned i = 0; i < count; i += sizeof(struct inotify_event) + event->len) {
            event = (struct inotify_event *)(buffer + i);
            char path[PATH_MAX];
            snprintf(path, PATH_MAX, "%s/%s", inotify_watches[event->wd].c_str(), event->name);
            callback(path);
        }
    }
}
