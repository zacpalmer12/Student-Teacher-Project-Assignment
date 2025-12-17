Implemented CheckAlloc.cpp as an extra feature to check the allocation against the stability and legality rules in the brief.

it reads the staff, projects and students files then stores them in STL containers.

if it cant open any file it prints invalid.

reads the file line by line each line must be studentid projectID supervisorID apart from the score line at the end during this each student must appear once and any unknown ids will cause it to produce INVALID.

it counts how many students are assigned to each project if this exceeds the multiplicity it produces INVALID.

it counts how many students are supervised by each supervisor if this exceeds their capacity it produces INVALID.

for each student looks at the projects they ranked higher than their assignment if any of these still have capacity this produces INVALID.

for a pair of students if one prefers the others over theirs and vice versa, with swapping improving both this would produce INVALID.

if a student is applied to a project they didn't choose checks whether the supervisor prefers any available project over the assigned one this means any of their own proposed projects, ones in their expertise or other projects this produces INVALID.

for a pair of supervisors compare them and if one prefers the other students project and vice versa, with swapping improving both this produces INVALID.

if any legality or stability rule fails outputs INVALID.

if all checks pass outputs VALID.