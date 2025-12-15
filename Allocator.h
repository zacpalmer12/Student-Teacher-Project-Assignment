#pragma once
using namespace std;
#include <unordered_map>
#include <vector>
#include "Staff.h"
#include "Project.h"
#include "Student.h"

void allocate(
    vector<Student>& students,
    unordered_map<int, Project>& projects,
    unordered_map<string, Staff>& staff
);

