#pragma once
using namespace std;

#include <unordered_map>
#include <vector>
#include "Staff.h"
#include "Project.h"
#include "Student.h"

int computeScore(
    const vector<Student>& students,
    const unordered_map<int, Project>& projects,
    const unordered_map<string, Staff>& staff
);

