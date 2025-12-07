#include "task.h"
#include <sstream>

TaskQueue::TaskQueue() {
    front = rear = nullptr;
}

TaskQueue::~TaskQueue() {
    TaskNode* temp;
    while (front) {
        temp = front;
        front = front->next;
        delete temp;
    }
}

void TaskQueue::assignTask(int id, string name, string eventName, string assignedTo, string dueDate, string description) {
    TaskNode* newNode = new TaskNode{id, name, eventName, assignedTo, dueDate, description, "pending", nullptr};
    if (!rear) {
        front = rear = newNode;
    } else {
        rear->next = newNode;
        rear = newNode;
    }
}

void TaskQueue::viewTasks() {
    TaskNode* temp = front;
    cout << "\n--- Tasks ---\n";
    while (temp) {
        cout << "ID: " << temp->id
             << ", Task: " << temp->name
             << ", Event: " << temp->eventName
             << ", Assigned To: " << temp->assignedTo
             << ", Due Date: " << temp->dueDate
             << ", Status: " << temp->status << endl;
        temp = temp->next;
    }
}

TaskNode* TaskQueue::getTask(int id) {
    TaskNode* temp = front;
    while (temp) {
        if (temp->id == id) return temp;
        temp = temp->next;
    }
    return nullptr;
}

void TaskQueue::saveToFile() {
    ofstream file("tasks.txt");
    TaskNode* temp = front;
    while (temp) {
        file << temp->id << "," << temp->name << "," << temp->eventName << "," 
             << temp->assignedTo << "," << temp->dueDate << "," << temp->description << "," 
             << temp->status << endl;
        temp = temp->next;
    }
    file.close();
}

void TaskQueue::loadFromFile() {
    ifstream file("tasks.txt");
    if (!file) return;

    string line;
    int id;
    string name, eventName, assignedTo, dueDate, description, status;

    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, line, ','); id = stoi(line);
        getline(ss, name, ',');
        getline(ss, eventName, ',');
        getline(ss, assignedTo, ',');
        getline(ss, dueDate, ',');
        getline(ss, description, ',');
        getline(ss, status, ',');
        assignTask(id, name, eventName, assignedTo, dueDate, description);
        TaskNode* t = getTask(id);
        if (t) t->status = status;
    }
    file.close();
}
