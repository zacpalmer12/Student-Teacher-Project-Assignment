#pragma once
#include <string>

using namespace std;
struct Project {
    int id;
    string proposer;
    int multiplicity;
    int assigned = 0;
    string subject;
};
