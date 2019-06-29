/* June 23, 2019 */

#include "tree.h"

module_t modules[] = {
    { .name = "fim", .main = fim_main },
    { .name = "logcollector", .main = logcollector_main },
    { .name = "sca", .main = sca_main },
    { .name = "open-scap", .main = openscap_main },
    { .name = "osquery", .main = osquery_main },
    { .name = "rootcheck", .main = rootcheck_main },
    { .name = "cis-cat", .main = ciscat_main },
    { .name = "inventory", .main = inventory_main },
    { .name = "aws", .main = aws_main },
    { .name = "azure", .main = azure_main },
    { .name = "docker", .main = docker_main },
    { .name = "command", .main = command_main },
    { .name = NULL, NULL }
};

static module_t agent = { .name = "agent" };
module_t * cur_module = &agent;

void handler(int signum) {
    switch (signum) {
    case SIGINT:
    case SIGTERM:
        print_info("agent", "Closing service");
        break;
    default:
        print_info("", "Signaled: %s", strsignal(signum));
    }
    exit(EXIT_SUCCESS);
}

void set_handler(int signum, void (*handler)(int)) {
    struct sigaction sa = { .sa_handler = handler };

    if (sigaction(signum, &sa, NULL) == -1) {
        critical("agent", "sigaction");
    }
}

void kill_modules() {
    int status;

    for (module_t * m = modules; m->name; m++) {
        kill(m->pid, SIGTERM);

        if (waitpid(m->pid, &status, 0) == -1) {
            critical("agent", "waitpid");
        }
    }
}

void spawn(module_t * m) {
    int stdin[2];
    int stdout[2];
    int sock[2];

    if (pipe(stdin) == -1 || pipe(stdout) == -1) {
        critical("agent", "Cannot create a pipe");
    }

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sock) == -1) {
        critical("agent", "Cannot create a socket");
    }

    m->pid = fork();

    switch (m->pid) {
    case -1:
        critical("agent", "Cannot create a subprocess");

    case 0:
        // Child

        // Restore signals
        set_handler(SIGINT, SIG_DFL);
        set_handler(SIGTERM, SIG_DFL);

        // Set stdin and stdout

        dup2(stdin[0], STDIN_FILENO);
        close(stdin[0]);
        close(stdin[1]);

        dup2(stdout[1], STDOUT_FILENO);
        close(stdout[0]);
        close(stdout[1]);

        m->sock = sock[0];
        close(sock[1]);

        // Run
        _exit(m->main(m));

    default:
        // Parent

        m->stdin = stdin[1];
        close(stdin[0]);
        cloexec(stdin[1]);

        m->stdout = stdout[0];
        cloexec(stdout[0]);
        close(stdout[1]);

        close(sock[0]);
        m->sock = sock[1];
    }
}

void dispatch() {
    fd_set rfds;
    int nfds = 0;
    struct timeval timeout = { .tv_sec = 1 };

    FD_ZERO(&rfds);

    for (module_t * m = modules; m->name; m++) {
        if (m->pid == 0) {
            continue;
        }

        FD_SET(m->stdout, &rfds);

        if (m->stdout >= nfds) {
            nfds = m->stdout + 1;
        }
    }

    switch (select(nfds, &rfds, NULL, NULL, &timeout)) {
    case -1:
        critical("agent", "select");

    case 0:
        break;

    default:
        for (module_t * m = modules; m->name; m++) {
            if (m->pid != 0 && FD_ISSET(m->stdout, &rfds)) {
                switch (read_msg(m->stdout, &m->stdout_buf)) {
                case -1:
                    // Messge incomplete
                    continue;

                case 0:
                    watchdog(m);
                    continue;
                }

                print_info("agent", "[%s] %s", m->name, m->stdout_buf.data);
                buffer_clear(&m->stdout_buf);
            }
        }
    }
}

void watchdog(module_t * m) {
    int status;

    switch (waitpid(m->pid, &status, WNOHANG)) {
    case -1:
        critical("agent", "waitpid");

    case 0:
        break;

    default:
        close(m->stdin);
        close(m->stdout);
        close(m->sock);

        if (WIFEXITED(status)) {
            print_warn("agent", "%s exited with code %d", m->name, WEXITSTATUS(status));
            m->pid = 0;
        } else if (WIFSIGNALED(status)) {
            print_warn("agent", "%s terminated by signal %s. Restarting module", m->name, strsignal(WTERMSIG(status)));
            spawn(m);
        }
    }
}

void critical(const char * tag, const char * s) {
    print_critical(tag, "%s: %s", s, strerror(errno));
    abort();
}

void cloexec(int fd) {
    int flags = fcntl(fd, F_GETFD);

    if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1) {
        critical("agent", "Cannot set file descriptor flag");
    }
}

void nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL);

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        critical("agent", "Cannot set file descriptor flag");
    }
}

void set_name(const char * name) {
    char buffer[16];

    snprintf(buffer, sizeof(buffer), "agent-%s", name);

    if (prctl(PR_SET_NAME, buffer, 0, 0, 0) == -1) {
        critical(name, "prctl");
    }
}

void write_msg(int fd, const char * data, size_t count) {
    write(fd, &count, sizeof(count));
    write(fd, data, count);
}

ssize_t read_msg(int fd, buffer_t * buffer) {
    ssize_t count;

    if (buffer->recv == 0) {
        // New message
        count = read(fd, &buffer->length, sizeof(size_t));

        switch (count) {
        case -1:
        case 0:
            return count;
        }

        if (count != sizeof(size_t)) {
            print_critical(cur_module->name, "Receiving %zd / %zu bytes. Pipe is not atomic!", count, sizeof(size_t));
            abort();
        }

        // Extend buffer

        if (buffer->length > buffer->alloc) {
            buffer->data = realloc(buffer->data, buffer->length + 1);

            if (buffer->data == NULL) {
                critical(cur_module->name, "Cannot extend memory");
            }

            buffer->alloc = buffer->length;
        }
    }

    // Read payload

    count = read(fd, buffer->data + buffer->recv, buffer->length - buffer->recv);

    switch (count) {
    case -1:
    case 0:
        return count;
    }

    buffer->recv += count;

    if (buffer->recv == buffer->length) {
        buffer->recv = 0;
        buffer->data[buffer->length] = '\0';
        return count;
    } else {
        return -1;
    }
}

void buffer_clear(buffer_t * buffer) {
    free(buffer->data);
    memset(buffer, 0, sizeof(buffer_t));
}
