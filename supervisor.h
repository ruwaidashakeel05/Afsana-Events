#ifndef SUPERVISOR_H
#define SUPERVISOR_H

#include "staff.h"
#include "task.h"
#include "review.h"
#include <iostream>
using namespace std;

class Supervisor {
private:
    StaffList staffList;
    TaskQueue taskQueue;
    ReviewStack reviewStack;

public:
    void loadAllData();
    void saveAllData();

    void staffManagementMenu();
    void taskAssignmentMenu();
    void reviewMenu();
    void mainMenu();
};

#endif
