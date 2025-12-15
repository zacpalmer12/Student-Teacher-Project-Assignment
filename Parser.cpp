#include "Parser.h"
using namespace std;
#include <fstream>
#include <sstream>
#include <iostream>

void parseStaff(
    const string& filename,
    unordered_map<string, Staff>& staff
) {
    ifstream file(filename);
    if (!file) {
        cerr << "Failed to open staff file\n";
        return;
    }

    string id;
    int load;
    string area;

    while (file >> id >> load) {
        Staff s;
        s.id = id;
        s.load = load;

        getline(file, area); // rest of line
        istringstream iss(area);
        string expertise;

        while (iss >> expertise) {
            s.expertise.insert(expertise);
        }

        staff[id] = s;
    }
}

void parseProjects(
    const string& filename,
    unordered_map<int, Project>& projects
) {
    ifstream file(filename);
    if (!file) {
        cerr << "Failed to open projects file\n";
        return;
    }

    int id, multiplicity;
    string proposer, subject, title;

    while (file >> id >> proposer >> multiplicity >> subject) {
        getline(file, title); // project title (with spaces)

        Project p;
        p.id = id;
        p.proposer = proposer;
        p.multiplicity = multiplicity;
        p.subject = subject;

        projects[id] = p;
    }
}

void parseStudents(
    const string& filename,
    vector<Student>& students
) {
    ifstream file(filename);
    if (!file) {
        cerr << "Failed to open students file\n";
        return;
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);

        Student s;
        iss >> s.id;

        int choice;
        while (iss >> choice) {
            s.choices.push_back(choice);
        }

        students.push_back(s);
    }
}
