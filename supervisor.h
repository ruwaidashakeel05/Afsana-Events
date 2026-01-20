#ifndef SUPERVISOR_H
#define SUPERVISOR_H

#include "staff.h"
#include "task.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

struct SupervisorNode {
    int id;
    string name;
    string email;
    string password;
    string phone;
    string role;
    string address;
    int assignedEvents;  // Number of confirmed events assigned
    SupervisorNode* next;
};

// Task assignment structure for supervisor-staff communication
struct TaskAssignment {
    int id;
    int supervisorId;
    int staffId;
    string description;
    string taskDetails;  // Detailed description/notes
    string location;
    string status;      // pending, assigned, completed
    string dueDate;
    string createdDate;
    TaskAssignment* next;
};

class SupervisorList {
private:
    SupervisorNode* head;
    string filename;

public:
    SupervisorList(string file = "supervisor.txt");
    ~SupervisorList();
    
    void loadFromFile();
    void saveToFile();
    
    int addSupervisor(int id, string name, string email, string password, string phone, string role, string address);
    SupervisorNode* findById(int id);
    SupervisorNode* login(string email, string password);
    string getSupervisorJSON(SupervisorNode* supervisor);
    string getAllSupervisorsJSON();
    bool deleteSupervisor(int id);
    bool updateSupervisor(int id, string name, string email, string phone, string role);
    int getNextId();
    bool incrementAssignedEvents(int supervisorId);
};

class TaskAssignmentList {
private:
    TaskAssignment* head;
    string filename;
    int nextId;

public:
    TaskAssignmentList(string file = "tasks.txt");
    ~TaskAssignmentList();
    
    void loadFromFile();
    void saveToFile();
    
    int createTask(int supervisorId, int staffId, string description, string taskDetails, string location, string dueDate);
    TaskAssignment* findTaskById(int id);
    TaskAssignment* findTasksByStaffId(int staffId);
    TaskAssignment* findTasksBySupervisorId(int supervisorId);
    bool updateTaskStatus(int taskId, string status);
    bool deleteTask(int taskId);
    string getTaskJSON(TaskAssignment* task);
    string getAllTasksJSON();
    string getStaffTasksJSON(int staffId);
    int getNextTaskId();
};

class Supervisor {
private:
    StaffList staffList;
    TaskAssignmentList taskList;

public:
    void loadAllData();
    void saveAllData();

    void staffManagementMenu();
    void taskAssignmentMenu();
    void mainMenu();
};

#endif
