#pragma once
using namespace std;
#include <string>
#include <vector>

struct Student {
    string id;
    vector<int> choices;
    int assignedProject = -1;
    string assignedSupervisor;
};

