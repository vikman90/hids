// June 22, 2024

#include <iostream>
#include "../include/io.hpp"

using namespace std;

void BasicIO::pushStateful(const string & data) {
    cout << "\e[95m[stateful]\e[39m " << data << endl;
}

void BasicIO::pushStateless(const string & data) {
    cout << "\e[94m[stateless]\e[39m " << data << endl;
}

string BasicIO::popCommand() {
    string buffer;
    getline(cin, buffer);
    return buffer;
}
