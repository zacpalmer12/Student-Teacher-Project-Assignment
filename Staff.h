#pragma once
#include <string>
#include <set>

struct Staff {
    std::string id;
    int load;
    int assigned = 0;
    std::set<std::string> expertise;
};
