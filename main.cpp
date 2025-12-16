#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Allocator.h"
#include "Parser.h"
#include "Score.h"

#include "Project.h"
#include "Staff.h"
#include "Student.h"

namespace
{
    void PrintUsage(std::ostream& os)
    {
        os << "Usage: ./GenAlloc staff.txt projects.txt students.txt alloc.txt\n";
    }

    void WriteOutput(
        const std::string& outFile,
        const std::vector<Student>& students,
        int score)
    {
        std::ofstream out(outFile);
        if (!out) {
            throw std::runtime_error("Failed to open output file: " + outFile);
        }

        for (const auto& s : students) {
            out << s.id << ' ' << s.assignedProject << ' ' << s.assignedSupervisor << '\n';
        }
        out << score << '\n';
    }
}

int main(int argc, char* argv[])
{
    if (argc != 5) {
        PrintUsage(std::cerr);
        return 1;
    }

    const std::string staffFile = argv[1];
    const std::string projectsFile = argv[2];
    const std::string studentsFile = argv[3];
    const std::string outFile = argv[4];

    std::unordered_map<std::string, Staff> staff;
    std::unordered_map<int, Project> projects;
    std::vector<Student> students;

    parseStaff(staffFile, staff);
    parseProjects(projectsFile, projects);
    parseStudents(studentsFile, students);

    allocate(students, projects, staff);

    const int score = computeScore(students, projects, staff);

    std::sort(
        students.begin(),
        students.end(),
        [](const Student& a, const Student& b) { return a.id < b.id; });

    try {
        WriteOutput(outFile, students, score);
    }
    catch (const std::exception& ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }

    return 0;
}
