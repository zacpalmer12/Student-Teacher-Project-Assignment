#pragma once
#include <string>
#include <vector>

struct Student {
    std::string id;
    std::vector<int> choices;
    int assignedProject = -1;
    std::string assignedSupervisor;
};

