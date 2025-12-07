#ifndef TASK_H
#define TASK_H

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

struct TaskNode {
    int id;
    string name;
    string eventName;
    string assignedTo;
    string dueDate;
    string description;
    string status; // pending, in-progress, completed
    TaskNode* next;
};

class TaskQueue {
private:
    TaskNode* front;
    TaskNode* rear;

public:
    TaskQueue();
    ~TaskQueue();

    void assignTask(int id, string name, string eventName, string assignedTo, string dueDate, string description);
    void viewTasks();
    TaskNode* getTask(int id);

    void saveToFile();
    void loadFromFile();
};

#endif
