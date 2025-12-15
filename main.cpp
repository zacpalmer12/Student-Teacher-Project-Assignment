#include <iostream>
#include <unordered_map>
#include <vector>

#include "Parser.h"
#include "Staff.h"
#include "Project.h"
#include "Student.h"

int main(int argc, char* argv[]) {
    // Check command-line arguments
    if (argc != 5) {
        std::cerr << "Usage: ./GenAlloc staff.txt projects.txt students.txt alloc.txt\n";
        return 1;
    }

    // Data containers
    std::unordered_map<std::string, Staff> staff;
    std::unordered_map<int, Project> projects;
    std::vector<Student> students;

    // Parse input files
    parseStaff(argv[1], staff);
    parseProjects(argv[2], projects);
    parseStudents(argv[3], students);

    // Debug output to verify parsing
    std::cout << staff.size() << " staff\n";
    std::cout << projects.size() << " projects\n";
    std::cout << students.size() << " students\n";

    return 0;
}
