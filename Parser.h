#pragma once
#include <unordered_map>
#include <vector>
#include "Staff.h"
#include "Project.h"
#include "Student.h"

void parseStaff(
    const std::string& filename,
    std::unordered_map<std::string, Staff>& staff
);

void parseProjects(
    const std::string& filename,
    std::unordered_map<int, Project>& projects
);

void parseStudents(
    const std::string& filename,
    std::vector<Student>& students
);
