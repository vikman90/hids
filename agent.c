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
                char buffer[BUFFER_SIZE];
                size_t offset = 0;
                ssize_t count = read(m->stdout, buffer + offset, sizeof(buffer) - offset - 1);

                switch (count) {
                case -1:
                    critical("agent", "Cannot read module's stdin");

                case 0:
                    print_warn("agent", "Cannot communicate with %s", m->name);
                    watchdog(m);
                    continue;

                default:
                    buffer[offset + count] = '\0';
                    char * token = strtok(buffer + offset, "\n");

                    while (token) {
                        print_info("agent", "[%s] %s", m->name, token);
                        offset = token + strlen(token) - buffer;
                        token = strtok(NULL, "\n");
                    }
                }
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
        if (WIFEXITED(status)) {
            print_warn("agent", "%s exited with code %d", m->name, WEXITSTATUS(status));
            m->pid = 0;
            close(m->stdin);
            close(m->stdout);
            close(m->sock);
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
