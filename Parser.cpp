#include "Parser.h"
#include <fstream>
#include <sstream>
#include <iostream>

void parseStaff(
    const std::string& filename,
    std::unordered_map<std::string, Staff>& staff
) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open staff file\n";
        return;
    }

    std::string id;
    int load;
    std::string area;

    while (file >> id >> load) {
        Staff s;
        s.id = id;
        s.load = load;

        // rest of line
        std::getline(file, area);
        std::istringstream iss(area);
        std::string expertise;

        while (iss >> expertise) {
            s.expertise.insert(expertise);
        }

        staff[id] = s;
    }
}

void parseProjects(
    const std::string& filename,
    std::unordered_map<int, Project>& projects
) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open projects file\n";
        return;
    }

    int id, multiplicity;
    std::string proposer, subject, title;

    while (file >> id >> proposer >> multiplicity >> subject) {
        // project title (with spaces)
        std::getline(file, title); 

        Project p;
        p.id = id;
        p.proposer = proposer;
        p.multiplicity = multiplicity;
        p.subject = subject;

        projects[id] = p;
    }
}

void parseStudents(
    const std::string& filename,
    std::vector<Student>& students
) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open students file\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);

        Student s;
        iss >> s.id;

        int choice;
        while (iss >> choice) {
            s.choices.push_back(choice);
        }

        students.push_back(s);
    }
}
