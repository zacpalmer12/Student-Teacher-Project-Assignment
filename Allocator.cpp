#include "Allocator.h"

#include <unordered_set>
#include <string>

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
        std::unordered_map<int, Project>& projects)
    {
        if (s.assignedProject != -1)
            return true;

        for (auto& kv : projects) {
            Project& p = kv.second;
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

        auto pit = projects.find(s.assignedProject);
        if (pit == projects.end())
            return false;

        if (!s.assignedSupervisor.empty())
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
    // -------------------------
    // Phase 1: Student -> Project using preferences
    // -------------------------
    for (auto& student : students) {
        if (student.assignedProject != -1)
            continue;

        for (int pid : student.choices) {
            if (AssignProjectIfNeeded(student, projects, pid))
                break;
        }
    }

    // -------------------------
    // Phase 2: Staff -> Students (assign supervisors; can also allocate projects to unassigned students)
    // Passes: own projects -> expertise -> any
    // -------------------------

    // Build quick index: projectId -> list of student indices assigned to that project
    // We'll rebuild this lazily when needed by scanning; simplicity over micro-optimisation.

    // 2.1 Own projects
    for (auto& stPair : staff) {
        Staff& st = stPair.second;

        if (!CanSupervise(st))
            continue;

        // First, supervise students already on st's projects
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

        // Then, if still capacity, allocate + supervise unassigned students onto st's projects
        for (auto& s : students) {
            if (!CanSupervise(st))
                break;

            if (!s.assignedSupervisor.empty())
                continue;

            if (s.assignedProject == -1) {
                // try put them on one of st's projects with capacity
                for (auto& pkv : projects) {
                    Project& p = pkv.second;
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

    // 2.2 Expertise projects
    for (auto& stPair : staff) {
        Staff& st = stPair.second;

        while (CanSupervise(st)) {
            bool didSomething = false;

            // supervise students already allocated to projects in expertise
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

            // allocate + supervise unassigned students into expertise projects if needed
            for (auto& s : students) {
                if (!CanSupervise(st))
                    break;

                if (!s.assignedSupervisor.empty())
                    continue;

                if (s.assignedProject == -1) {
                    for (auto& pkv : projects) {
                        Project& p = pkv.second;
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

    // 2.3 Any projects
    for (auto& stPair : staff) {
        Staff& st = stPair.second;

        while (CanSupervise(st)) {
            bool didSomething = false;

            // supervise any already-allocated students who still lack a supervisor
            for (auto& s : students) {
                if (!CanSupervise(st))
                    break;

                if (s.assignedProject == -1 || !s.assignedSupervisor.empty())
                    continue;

                if (TryAssignSupervisorToStudent(s, st, projects)) {
                    didSomething = true;
                }
            }

            // allocate + supervise any remaining unassigned students
            for (auto& s : students) {
                if (!CanSupervise(st))
                    break;

                if (!s.assignedSupervisor.empty())
                    continue;

                if (s.assignedProject == -1) {
                    if (!EnsureStudentHasAnyProject(s, projects))
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

    // -------------------------
    // Final safety: ensure every student has a project and supervisor (fallback)
    // -------------------------
    // Ensure projects for any still unassigned
    for (auto& s : students) {
        EnsureStudentHasAnyProject(s, projects);
    }

    // Ensure supervisors for any still without one
    for (auto& s : students) {
        if (!s.assignedSupervisor.empty())
            continue;

        for (auto& stPair : staff) {
            Staff& st = stPair.second;
            if (TryAssignSupervisorToStudent(s, st, projects))
                break;
        }
    }
}
