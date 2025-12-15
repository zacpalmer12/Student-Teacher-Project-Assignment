#pragma once
#include <string>

struct Project {
    int id;
    std::string proposer;
    int multiplicity;
    int assigned = 0;
    std::string subject;
};
