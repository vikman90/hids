// June 23, 2019
// June 23, 2024

#pragma once

#include <string>

struct FIMItem {
    FIMItem(const std::string & path) : path(path) { }

    const std::string path;
    bool warn;
};
