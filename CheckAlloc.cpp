#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Parser.h"
#include "Project.h"
#include "Staff.h"
#include "Student.h"

struct Assignment {
    int projectId = -1;
    std::string supervisorId;
};

namespace
{
    constexpr int kNoRank = 1'000'000;

    int StudentRankForProject(const Student& s, int pid)
    {
        for (int i = 0; i < static_cast<int>(s.choices.size()); ++i) {
            if (s.choices[i] == pid)
                return i; // 0..3
        }
        return kNoRank; // not on list
    }

    bool StudentStrictlyPrefers(const Student& s, int pidA, int pidB)
    {
        // True if s ranks pidA higher than pidB, and pidA is actually on their list.
        const int ra = StudentRankForProject(s, pidA);
        if (ra == kNoRank)
            return false;

        const int rb = StudentRankForProject(s, pidB);
        return ra < rb;
    }

    // Supervisor "preference category" for supervising a given project:
    // 0 = own proposal (best), 1 = expertise, 2 = neither (worst)
    int SupervisorCategoryForProject(const Staff& st, const Project& p)
    {
        if (p.proposer == st.id)
            return 0;
        if (st.expertise.find(p.subject) != st.expertise.end())
            return 1;
        return 2;
    }

    bool SupervisorStrictlyPrefersProject(const Staff& st, const Project& better, const Project& worse)
    {
        return SupervisorCategoryForProject(st, better) < SupervisorCategoryForProject(st, worse);
    }
}

int main(int argc, char* argv[])
{
    if (argc != 5) {
        std::cerr << "Usage: ./CheckAlloc staff.txt projects.txt students.txt alloc.txt\n";
        return 1;
    }

    std::unordered_map<std::string, Staff> staff;
    std::unordered_map<int, Project> projects;
    std::vector<Student> students;

    parseStaff(argv[1], staff);
    parseProjects(argv[2], projects);
    parseStudents(argv[3], students);

    // studentId -> Student*
    std::unordered_map<std::string, const Student*> studentsById;
    studentsById.reserve(students.size());
    for (const auto& s : students) {
        studentsById[s.id] = &s;
    }

    std::ifstream in(argv[4]);
    if (!in) {
        std::cerr << "Cannot open allocation file\n";
        return 1;
    }

    // Read alloc file (ignore final score-only line if present)
    std::unordered_map<std::string, Assignment> alloc;
    alloc.reserve(students.size());

    std::unordered_set<std::string> seenStudents;
    seenStudents.reserve(students.size());

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty())
            continue;

        std::istringstream iss(line);

        std::string sid;
        int pid = -1;
        std::string sup;

        // Either: sid pid sup   OR: score-only line
        if (!(iss >> sid)) {
            continue;
        }

        // If line is just a single integer, it's probably the score line.
        // Detect: first token is all digits and no other tokens.
        bool allDigits = !sid.empty() && std::all_of(sid.begin(), sid.end(), ::isdigit);
        if (allDigits) {
            // score line -> ignore
            continue;
        }

        if (!(iss >> pid >> sup)) {
            std::cout << "INVALID\n";
            return 0;
        }

        // Student must exist
        if (studentsById.find(sid) == studentsById.end()) {
            std::cout << "INVALID\n";
            return 0;
        }

        // No duplicate student lines
        if (!seenStudents.insert(sid).second) {
            std::cout << "INVALID\n";
            return 0;
        }

        alloc[sid] = { pid, sup };
    }

    // Must allocate every student exactly once
    if (alloc.size() != students.size()) {
        std::cout << "INVALID\n";
        return 0;
    }

    // -------------------------
    // LEGALITY CHECKS
    // -------------------------
    std::unordered_map<int, int> projectCount;
    std::unordered_map<std::string, int> staffCount;

    projectCount.reserve(projects.size());
    staffCount.reserve(staff.size());

    for (const auto& kv : alloc) {
        const Assignment& a = kv.second;

        if (projects.find(a.projectId) == projects.end()) {
            std::cout << "INVALID\n";
            return 0;
        }
        if (staff.find(a.supervisorId) == staff.end()) {
            std::cout << "INVALID\n";
            return 0;
        }

        projectCount[a.projectId]++;
        staffCount[a.supervisorId]++;
    }

    for (const auto& kv : projectCount) {
        const Project& p = projects.at(kv.first);
        if (kv.second > p.multiplicity) {
            std::cout << "INVALID\n";
            return 0;
        }
    }

    for (const auto& kv : staffCount) {
        const Staff& st = staff.at(kv.first);
        if (kv.second > st.load) {
            std::cout << "INVALID\n";
            return 0;
        }
    }

    // Helper: is project available (not full)?
    auto isAvailable = [&](int pid) -> bool {
        const auto pit = projects.find(pid);
        if (pit == projects.end())
            return false;
        const int used = projectCount.count(pid) ? projectCount.at(pid) : 0;
        return used < pit->second.multiplicity;
        };

    // Build supervisor -> supervisee list (for stability rule 4)
    std::unordered_map<std::string, std::vector<std::string>> superviseesByStaff;
    superviseesByStaff.reserve(staff.size());
    for (const auto& kv : alloc) {
        superviseesByStaff[kv.second.supervisorId].push_back(kv.first);
    }

    // -------------------------
    // STABILITY RULE 1:
    // No student should get a lower/not-chosen project if a higher choice is still available.
    // -------------------------
    for (const auto& s : students) {
        const Assignment& a = alloc.at(s.id);
        const int currentPid = a.projectId;
        const int curRank = StudentRankForProject(s, currentPid);

        // For any choice ranked above current, if available -> unstable
        for (int i = 0; i < static_cast<int>(s.choices.size()); ++i) {
            if (i >= curRank)
                break;
            const int preferredPid = s.choices[i];
            if (isAvailable(preferredPid)) {
                std::cout << "INVALID\n";
                return 0;
            }
        }
    }

    // -------------------------
    // STABILITY RULE 2:
    // No two students would both prefer swapping allocated projects.
    // -------------------------
    // Build ordered list of ids for deterministic pair iteration
    std::vector<std::string> ids;
    ids.reserve(students.size());
    for (const auto& s : students) {
        ids.push_back(s.id);
    }

    for (std::size_t i = 0; i < ids.size(); ++i) {
        const Student& A = *studentsById.at(ids[i]);
        const int pidA = alloc.at(A.id).projectId;

        for (std::size_t j = i + 1; j < ids.size(); ++j) {
            const Student& B = *studentsById.at(ids[j]);
            const int pidB = alloc.at(B.id).projectId;

            if (StudentStrictlyPrefers(A, pidB, pidA) &&
                StudentStrictlyPrefers(B, pidA, pidB))
            {
                std::cout << "INVALID\n";
                return 0;
            }
        }
    }

    // -------------------------
    // STABILITY RULE 3:
    // No supervisor should supervise student+project when:
    // - student's allocated project is NOT on student's top 4 choices, AND
    // - supervisor has some other AVAILABLE project that they prefer more.
    // (Switching project wouldn't make student more unhappy than already.)
    // -------------------------
    for (const auto& s : students) {
        const Assignment& a = alloc.at(s.id);

        const Staff& st = staff.at(a.supervisorId);
        const Project& curProj = projects.at(a.projectId);

        const int studentRank = StudentRankForProject(s, a.projectId);
        const bool studentDidNotChoose = (studentRank == kNoRank);

        if (!studentDidNotChoose)
            continue;

        // Is there an available project that st strictly prefers over current?
        for (const auto& pkv : projects) {
            const Project& candidate = pkv.second;

            if (!isAvailable(candidate.id))
                continue;

            if (SupervisorStrictlyPrefersProject(st, candidate, curProj)) {
                std::cout << "INVALID\n";
                return 0;
            }
        }
    }

    // -------------------------
    // STABILITY RULE 4:
    // No two supervisors would both become happier after swapping one supervisee each.
    // Projects do NOT change; only supervisors swap students.
    // "Indifferent" swaps do NOT count as violation: both must be STRICTLY happier.
    // -------------------------
    // Iterate over pairs of supervisors that actually supervise someone
    std::vector<std::string> supervisingStaff;
    supervisingStaff.reserve(superviseesByStaff.size());
    for (const auto& kv : superviseesByStaff) {
        if (!kv.second.empty())
            supervisingStaff.push_back(kv.first);
    }

    for (std::size_t i = 0; i < supervisingStaff.size(); ++i) {
        const std::string& supAId = supervisingStaff[i];
        const Staff& supA = staff.at(supAId);
        const auto& supAStudents = superviseesByStaff.at(supAId);

        for (std::size_t j = i + 1; j < supervisingStaff.size(); ++j) {
            const std::string& supBId = supervisingStaff[j];
            const Staff& supB = staff.at(supBId);
            const auto& supBStudents = superviseesByStaff.at(supBId);

            // Try swapping one student from A with one from B
            for (const auto& studAId : supAStudents) {
                const int projAId = alloc.at(studAId).projectId;
                const Project& projA = projects.at(projAId);

                for (const auto& studBId : supBStudents) {
                    const int projBId = alloc.at(studBId).projectId;
                    const Project& projB = projects.at(projBId);

                    // A would supervise projB instead of projA; B would supervise projA instead of projB
                    const bool AStrictlyHappier = SupervisorStrictlyPrefersProject(supA, projB, projA);
                    const bool BStrictlyHappier = SupervisorStrictlyPrefersProject(supB, projA, projB);

                    if (AStrictlyHappier && BStrictlyHappier) {
                        std::cout << "INVALID\n";
                        return 0;
                    }
                }
            }
        }
    }

    std::cout << "VALID\n";
    return 0;
}
