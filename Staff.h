#pragma once
using namespace std;
#include <string>
#include <set>

struct Staff {
    string id;
    int load;
    int assigned = 0;
    set<std::string> expertise;
};
