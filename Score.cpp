#include "Score.h"
#include <algorithm>

int computeScore(
    const std::vector<Student>& students,
    const std::unordered_map<int, Project>& projects,
    const std::unordered_map<std::string, Staff>& staff)
{
    int score = 0;


    // student preference score
    for (const auto& s : students) {
        int prefScore = 0;

        for (std::size_t i = 0; i < s.choices.size(); ++i) {
            if (s.choices[i] == s.assignedProject) {
                prefScore = 4 - static_cast<int>(i);
                break;
            }
        }

        score += prefScore;
    }


    // Supervisor preference score
    for (const auto& s : students) {
        if (s.assignedProject == -1 || s.assignedSupervisor.empty())
            continue;

        const auto pit = projects.find(s.assignedProject);
        const auto sit = staff.find(s.assignedSupervisor);

        if (pit == projects.end() || sit == staff.end())
            continue;

        const Project& p = pit->second;
        const Staff& st = sit->second;

        if (p.proposer == st.id) {
            score += 4;
        }
        else if (st.expertise.find(p.subject) != st.expertise.end()) {
            score += 2;
        }
        // else we add nothing
    }

    return score;
}
