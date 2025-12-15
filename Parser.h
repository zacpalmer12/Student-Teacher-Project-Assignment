#pragma once
using namespace std;


#include <unordered_map>
#include <vector>
#include <string>

#include "Staff.h"
#include "Project.h"
#include "Student.h"

void parseStaff(
    const string& filename,
    unordered_map<string, Staff>& staff
);

void parseProjects(
    const string& filename,
    unordered_map<int, Project>& projects
);

void parseStudents(
    const string& filename,
    vector<Student>& students
);
