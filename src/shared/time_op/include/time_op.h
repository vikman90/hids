// June 23, 2019
// June 23, 2024

#pragma once

#define time_diff(x, y) ((x.tv_sec - y.tv_sec) + (x.tv_nsec - y.tv_nsec) / 1e9)
