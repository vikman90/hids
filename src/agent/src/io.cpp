// June 22, 2024

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <sys/select.h>
#include "../include/io.hpp"
#include <logger.hpp>

using namespace std;

void BasicIO::pushStateful(const string & data) {
    cout << "\e[95m[stateful]\e[39m " << data << endl;
}

void BasicIO::pushStateless(const string & data) {
    cout << "\e[94m[stateless]\e[39m " << data << endl;
}

string BasicIO::popCommand(int timeout_sec) {
    string buffer;
    fd_set rfds;
    struct timeval timeout = { .tv_sec = timeout_sec };

    FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO, &rfds);

    switch (select(STDIN_FILENO + 1, &rfds, NULL, NULL, &timeout)) {
    case -1:
        Logger::error("select: {}", strerror(errno));

    case 0:
        break;

    default:
        getline(cin, buffer);
    }

    return buffer;
}
