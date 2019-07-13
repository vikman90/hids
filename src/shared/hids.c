/* July 13, 2019 */

#include "hids.h"

void handler(int signum) {
    switch (signum) {
    case SIGINT:
    case SIGTERM:
        print_info("Closing service");
        break;
    default:
        print_info("Signaled: %s", strsignal(signum));
    }

    exit(EXIT_SUCCESS);
}

void set_handler(int signum, void (*handler)(int)) {
    struct sigaction sa = { .sa_handler = handler };

    if (sigaction(signum, &sa, NULL) == -1) {
        critical("sigaction");
    }
}

void kill_modules() {
    int status;

    for (module_t * m = modules; m->name; m++) {
        kill(m->pid, SIGTERM);

        if (waitpid(m->pid, &status, 0) == -1) {
            critical("waitpid");
        }
    }
}

void spawn(module_t * m) {
    int pipein[2];
    int pipeout[2];

    if (pipe(pipein) == -1 || pipe(pipeout) == -1) {
        critical("Cannot create a pipe");
    }

    m->pid = fork();

    switch (m->pid) {
    case -1:
        critical("Cannot create a subprocess");

    case 0:
        // Child

        // Restore signals
        set_handler(SIGINT, SIG_DFL);
        set_handler(SIGTERM, SIG_DFL);

        // Set stdin and stdout

        dup2(pipein[0], STDIN_FILENO);
        close(pipein[0]);
        close(pipein[1]);

        dup2(pipeout[1], STDOUT_FILENO);
        close(pipeout[0]);
        close(pipeout[1]);

        // Run
        execl(argv0, argv0, m->name, NULL);
        _exit(127);

    default:
        // Parent

        m->stdin = fdopen(pipein[1], "w");

        if (m->stdin == NULL) {
            critical("Cannot create file for pipe");
        }

        close(pipein[0]);
        cloexec(pipein[1]);

        m->stdout = fdopen(pipeout[0], "r");

        if (m->stdout == NULL) {
            critical("Cannot create file for pipe");
        }

        cloexec(pipeout[0]);
        close(pipeout[1]);
    }
}

void dispatch() {
    fd_set rfds;
    int nfds = 0;
    struct timeval timeout = { .tv_sec = 1 };

    FD_ZERO(&rfds);

    for (module_t * m = modules; m->name; m++) {
        int fd = fileno(m->stdout);

        if (m->pid == 0) {
            continue;
        }

        FD_SET(fd, &rfds);

        if (fd >= nfds) {
            nfds = fd + 1;
        }
    }

    switch (select(nfds, &rfds, NULL, NULL, &timeout)) {
    case -1:
        critical("select");

    case 0:
        break;

    default:
        for (module_t * m = modules; m->name; m++) {
            if (m->pid != 0 && FD_ISSET(fileno(m->stdout), &rfds)) {
                char buffer[BUFFER_SIZE];

                if (fgets(buffer, BUFFER_SIZE, m->stdout)) {
                    char * end = buffer + strlen(buffer) - 1;

                    if (*end == '\n') {
                        *end = '\0';
                        print_info("[%s] %s", m->name, buffer);
                    } else {
                        print_warn("%s sent an incomplete message", m->name);
                    }
                } else {
                    watchdog(m);
                }
            }
        }
    }
}

void watchdog(module_t * m) {
    int status;

    switch (waitpid(m->pid, &status, WNOHANG)) {
    case -1:
        critical("waitpid");

    case 0:
        break;

    default:
        fclose(m->stdin);
        fclose(m->stdout);

        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) != 0) {
                print_warn("%s exited with code %d", m->name, WEXITSTATUS(status));
            }

            m->pid = 0;
        } else if (WIFSIGNALED(status)) {
            print_warn("%s terminated by signal %s. Restarting module", m->name, strsignal(WTERMSIG(status)));
            spawn(m);
        }
    }
}

void critical(const char * s) {
    print_critical("%s: %s", s, strerror(errno));
    abort();
}

void cloexec(int fd) {
    int flags = fcntl(fd, F_GETFD);

    if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1) {
        critical("Cannot set file descriptor flag");
    }
}

void nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL);

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        critical("Cannot set file descriptor flag");
    }
}

void set_name(const char * name) {
    char buffer[16];

    snprintf(buffer, sizeof(buffer), "agent-%s", name);

    if (prctl(PR_SET_NAME, buffer, 0, 0, 0) == -1) {
        critical("prctl");
    }
}

void set_cwd() {
    char copy[PATH_MAX] = "";
    char path[PATH_MAX];

    strncpy(copy, argv0, PATH_MAX - 1);
    snprintf(path, PATH_MAX, "%s/..", dirname(copy));

    if (chdir(path) == -1) {
        critical("Cannot change directory");
    }
}

int file_sha256(int fd, char sum[SHA256_LEN]) {
    static const char HEX[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    static EVP_MD_CTX * ctx;

    if (ctx == NULL) {
        ctx = EVP_MD_CTX_create();
    }

    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);

    char buffer[BUFFER_SIZE];
    ssize_t count;

    while ((count = read(fd, buffer, BUFFER_SIZE)) > 0) {
        EVP_DigestUpdate(ctx, buffer, count);
    }

    if (count == -1) {
        return -1;
    }

    unsigned char md[SHA256_DIGEST_LENGTH];
    EVP_DigestFinal_ex(ctx, md, NULL);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        // sprintf(sum + i * 2, "%02x", md[i]);
        sum[i * 2] = HEX[md[i] >> 4];
        sum[i * 2 + 1] = HEX[md[i] & 0xF];
    }

    sum[SHA256_LEN - 1] = '\0';
    return 0;
}

void dispatch_stdin(time_t timeout_sec) {
    char buffer[BUFFER_SIZE];
    fd_set rfds;
    struct timeval timeout = { .tv_sec = timeout_sec };

    FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO, &rfds);

    switch (select(STDIN_FILENO + 1, &rfds, NULL, NULL, &timeout)) {
    case -1:
        critical("select");

    case 0:
        break;

    default:
        switch (read(STDIN_FILENO, buffer, sizeof(buffer))) {
        case -1:
            critical("Cannot read stdin");

        case 0:
            // Agent exited. Printing a message would raise SIGPIPE.
            exit(EXIT_SUCCESS);
        }
    }
}
