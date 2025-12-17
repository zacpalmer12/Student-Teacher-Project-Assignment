#pragma once
#include <unordered_map>
#include <vector>
#include "Staff.h"
#include "Project.h"
#include "Student.h"

void allocate(
    std::vector<Student>& students,
    std::unordered_map<int, Project>& projects,
    std::unordered_map<std::string, Staff>& staff
);

