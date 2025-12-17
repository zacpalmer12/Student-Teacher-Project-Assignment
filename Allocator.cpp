#include "Allocator.h"

#include <algorithm>
#include <string>
#include <vector>

namespace
{
    bool CanSupervise(const Staff& st)
    {
        return st.assigned < st.load;
    }

    bool AssignProjectIfNeeded(
        Student& s,
        std::unordered_map<int, Project>& projects,
        int pid)
    {
        auto it = projects.find(pid);
        if (it == projects.end())
            return false;

        Project& p = it->second;
        if (p.assigned >= p.multiplicity)
            return false;

        s.assignedProject = pid;
        p.assigned++;
        return true;
    }

    bool EnsureStudentHasAnyProject(
        Student& s,
        std::unordered_map<int, Project>& projects,
        const std::vector<int>& projectIds)
    {
        if (s.assignedProject != -1)
            return true;

        for (int pid : projectIds) {
            Project& p = projects.at(pid);
            if (p.assigned < p.multiplicity) {
                s.assignedProject = p.id;
                p.assigned++;
                return true;
            }
        }
        return false;
    }

    bool TryAssignSupervisorToStudent(
        Student& s,
        Staff& st,
        const std::unordered_map<int, Project>& projects)
    {
        if (!CanSupervise(st))
            return false;

        if (s.assignedProject == -1)
            return false;

        if (!s.assignedSupervisor.empty())
            return false;

        auto pit = projects.find(s.assignedProject);
        if (pit == projects.end())
            return false;

        s.assignedSupervisor = st.id;
        st.assigned++;
        return true;
    }
}

void allocate(
    std::vector<Student>& students,
    std::unordered_map<int, Project>& projects,
    std::unordered_map<std::string, Staff>& staff)
{
    // fixed order for staff and projects
    std::vector<std::string> staffIds;
    staffIds.reserve(staff.size());
    for (const auto& kv : staff) {
        staffIds.push_back(kv.first);
    }
    std::sort(staffIds.begin(), staffIds.end());

    std::vector<int> projectIds;
    projectIds.reserve(projects.size());
    for (const auto& kv : projects) {
        projectIds.push_back(kv.first);
    }
    std::sort(projectIds.begin(), projectIds.end());


    // Phase 1: Assign students to to projects by preference

    for (auto& student : students) {
        if (student.assignedProject != -1)
            continue;

        for (int pid : student.choices) {
            if (AssignProjectIfNeeded(student, projects, pid))
                break;
        }
    }

    // Phase 2: Assign supervisors

    // 2.1 Supervisors own projects
    for (const auto& sid : staffIds) {
        Staff& st = staff.at(sid);

        if (!CanSupervise(st))
            continue;

        // supervise students already on staff's projects
        for (auto& s : students) {
            if (!CanSupervise(st))
                break;

            if (s.assignedProject == -1 || !s.assignedSupervisor.empty())
                continue;

            auto pit = projects.find(s.assignedProject);
            if (pit == projects.end())
                continue;

            if (pit->second.proposer == st.id) {
                TryAssignSupervisorToStudent(s, st, projects);
            }
        }

        // Assign and supervise students on own projects
        for (auto& s : students) {
            if (!CanSupervise(st))
                break;

            if (!s.assignedSupervisor.empty())
                continue;

            if (s.assignedProject == -1) {
                for (int pid : projectIds) {
                    Project& p = projects.at(pid);
                    if (p.proposer == st.id && p.assigned < p.multiplicity) {
                        s.assignedProject = p.id;
                        p.assigned++;
                        TryAssignSupervisorToStudent(s, st, projects);
                        break;
                    }
                }
            }
        }
    }

    // 2.2 Expertise projects of supervisors
    for (const auto& sid : staffIds) {
        Staff& st = staff.at(sid);

        while (CanSupervise(st)) {
            bool didSomething = false;

            // supervise already allocated students in expertise
            for (auto& s : students) {
                if (!CanSupervise(st))
                    break;

                if (s.assignedProject == -1 || !s.assignedSupervisor.empty())
                    continue;

                auto pit = projects.find(s.assignedProject);
                if (pit == projects.end())
                    continue;

                const Project& p = pit->second;
                if (st.expertise.find(p.subject) != st.expertise.end()) {
                    if (TryAssignSupervisorToStudent(s, st, projects)) {
                        didSomething = true;
                    }
                }
            }

            // assign and supervise students on expertise projects
            for (auto& s : students) {
                if (!CanSupervise(st))
                    break;

                if (!s.assignedSupervisor.empty())
                    continue;

                if (s.assignedProject == -1) {
                    for (int pid : projectIds) {
                        Project& p = projects.at(pid);
                        if (p.assigned < p.multiplicity &&
                            st.expertise.find(p.subject) != st.expertise.end()) {

                            s.assignedProject = p.id;
                            p.assigned++;
                            if (TryAssignSupervisorToStudent(s, st, projects)) {
                                didSomething = true;
                            }
                            break;
                        }
                    }
                }
            }

            if (!didSomething)
                break;
        }
    }

    // 2.3 Any remaining projects
    for (const auto& sid : staffIds) {
        Staff& st = staff.at(sid);

        while (CanSupervise(st)) {
            bool didSomething = false;

            // supervise any students without a supervisor
            for (auto& s : students) {
                if (!CanSupervise(st))
                    break;

                if (s.assignedProject == -1 || !s.assignedSupervisor.empty())
                    continue;

                if (TryAssignSupervisorToStudent(s, st, projects)) {
                    didSomething = true;
                }
            }

            // Assign and supervise remaining unassigned students
            for (auto& s : students) {
                if (!CanSupervise(st))
                    break;

                if (!s.assignedSupervisor.empty())
                    continue;

                if (s.assignedProject == -1) {
                    if (!EnsureStudentHasAnyProject(s, projects, projectIds))
                        continue;

                    if (TryAssignSupervisorToStudent(s, st, projects)) {
                        didSomething = true;
                    }
                }
            }

            if (!didSomething)
                break;
        }
    }

    // Check all students have a project and supervisor

    for (auto& s : students) {
        EnsureStudentHasAnyProject(s, projects, projectIds);
    }

    for (auto& s : students) {
        if (!s.assignedSupervisor.empty())
            continue;

        for (const auto& sid : staffIds) {
            Staff& st = staff.at(sid);
            if (TryAssignSupervisorToStudent(s, st, projects))
                break;
        }
    }
}
