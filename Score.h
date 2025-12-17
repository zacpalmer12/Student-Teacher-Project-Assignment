#pragma once

#include <unordered_map>
#include <vector>
#include "Staff.h"
#include "Project.h"
#include "Student.h"

int computeScore(
    const std::vector<Student>& students,
    const std::unordered_map<int, Project>& projects,
    const std::unordered_map<std::string, Staff>& staff
);

