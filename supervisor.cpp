#include "supervisor.h"
#include "utils.h"

// ==================== SUPERVISOR LIST IMPLEMENTATION ====================

SupervisorList::SupervisorList(string file) : filename(file), head(nullptr) {
    loadFromFile();
}

SupervisorList::~SupervisorList() {
    SupervisorNode* current = head;
    while (current != nullptr) {
        SupervisorNode* next = current->next;
        delete current;
        current = next;
    }
}

void SupervisorList::loadFromFile() {
    SupervisorNode* current = head;
    while (current != nullptr) {
        SupervisorNode* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;

    ifstream file(filename);
    if (!file.is_open()) {
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        istringstream iss(line);
        int id;
        string name, email, password, phone, role, address;
        
        if (iss >> id) {
            iss.ignore();
            getline(iss, name, '|');
            getline(iss, email, '|');
            getline(iss, password, '|');
            getline(iss, phone, '|');
            getline(iss, role, '|');
            getline(iss, address, '|');
            
            SupervisorNode* newNode = new SupervisorNode();
            newNode->id = id;
            newNode->name = name;
            newNode->email = email;
            newNode->password = password;
            newNode->phone = phone;
            newNode->role = role;
            newNode->address = address;
            newNode->assignedEvents = 0;
            newNode->next = head;
            head = newNode;
        }
    }
    file.close();
}

void SupervisorList::saveToFile() {
    ofstream file(filename);
    if (!file.is_open()) {
        return;
    }

    SupervisorNode* current = head;
    while (current != nullptr) {
        file << current->id << "|"
             << current->name << "|"
             << current->email << "|"
             << current->password << "|"
             << current->phone << "|"
             << current->role << "|"
             << current->address << endl;
        current = current->next;
    }
    file.close();
}

int SupervisorList::addSupervisor(int id, string name, string email, string password, string phone, string role, string address) {
    SupervisorNode* newNode = new SupervisorNode();
    newNode->id = id;
    newNode->name = name;
    newNode->email = email;
    newNode->password = password;
    newNode->phone = phone;
    newNode->role = role;
    newNode->address = address;
    newNode->assignedEvents = 0;
    newNode->next = head;
    head = newNode;
    saveToFile();
    return id;
}

SupervisorNode* SupervisorList::findById(int id) {
    SupervisorNode* current = head;
    while (current != nullptr) {
        if (current->id == id) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

SupervisorNode* SupervisorList::login(string email, string password) {
    SupervisorNode* current = head;
    while (current != nullptr) {
        if (current->email == email && current->password == password) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

string SupervisorList::getSupervisorJSON(SupervisorNode* supervisor) {
    if (supervisor == nullptr) return "";
    
    string json = "{";
    json += "\"id\":" + to_string(supervisor->id) + ",";
    json += "\"name\":\"" + escapeJSON(supervisor->name) + "\",";
    json += "\"email\":\"" + escapeJSON(supervisor->email) + "\",";
    json += "\"phone\":\"" + escapeJSON(supervisor->phone) + "\",";
    json += "\"role\":\"" + escapeJSON(supervisor->role) + "\",";
    json += "\"address\":\"" + escapeJSON(supervisor->address) + "\",";
    json += "\"assignedEvents\":" + to_string(supervisor->assignedEvents);
    json += "}";
    return json;
}

string SupervisorList::getAllSupervisorsJSON() {
    string json = "[";
    SupervisorNode* current = head;
    bool first = true;
    
    while (current != nullptr) {
        if (!first) json += ",";
        json += "{";
        json += "\"id\":" + to_string(current->id) + ",";
        json += "\"name\":\"" + current->name + "\",";
        json += "\"email\":\"" + current->email + "\",";
        json += "\"phone\":\"" + current->phone + "\",";
        json += "\"role\":\"" + current->role + "\",";
        json += "\"address\":\"" + current->address + "\",";
        json += "\"assignedEvents\":" + to_string(current->assignedEvents);
        json += "}";
        first = false;
        current = current->next;
    }
    json += "]";
    return json;
}

bool SupervisorList::deleteSupervisor(int id) {
    if (head == nullptr) return false;
    
    if (head->id == id) {
        SupervisorNode* temp = head;
        head = head->next;
        delete temp;
        saveToFile();
        return true;
    }
    
    SupervisorNode* current = head;
    while (current->next != nullptr) {
        if (current->next->id == id) {
            SupervisorNode* temp = current->next;
            current->next = current->next->next;
            delete temp;
            saveToFile();
            return true;
        }
        current = current->next;
    }
    return false;
}

bool SupervisorList::updateSupervisor(int id, string name, string email, string phone, string role) {
    SupervisorNode* supervisor = findById(id);
    if (supervisor == nullptr) return false;
    
    supervisor->name = name;
    supervisor->email = email;
    supervisor->phone = phone;
    supervisor->role = role;
    saveToFile();
    return true;
}

int SupervisorList::getNextId() {
    int maxId = 0;
    SupervisorNode* current = head;
    while (current != nullptr) {
        if (current->id > maxId) {
            maxId = current->id;
        }
        current = current->next;
    }
    return maxId + 1;
}

bool SupervisorList::incrementAssignedEvents(int supervisorId) {
    SupervisorNode* supervisor = findById(supervisorId);
    if (supervisor == nullptr) return false;
    
    supervisor->assignedEvents++;
    saveToFile();
    return true;
}

// ==================== TASK ASSIGNMENT LIST IMPLEMENTATION ====================

TaskAssignmentList::TaskAssignmentList(string file) : filename(file), head(nullptr), nextId(1) {
    loadFromFile();
}

TaskAssignmentList::~TaskAssignmentList() {
    TaskAssignment* current = head;
    while (current != nullptr) {
        TaskAssignment* next = current->next;
        delete current;
        current = next;
    }
}

void TaskAssignmentList::loadFromFile() {
    TaskAssignment* current = head;
    while (current != nullptr) {
        TaskAssignment* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
    nextId = 1;

    ifstream file(filename);
    if (!file.is_open()) {
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        istringstream iss(line);
        int id, supervisorId, staffId;
        string description, taskDetails, location, status, dueDate, createdDate;
        
        if (iss >> id) {
            iss.ignore();
            iss >> supervisorId;
            iss.ignore();
            iss >> staffId;
            iss.ignore();
            getline(iss, description, '|');
            getline(iss, taskDetails, '|');
            getline(iss, location, '|');
            getline(iss, status, '|');
            getline(iss, dueDate, '|');
            getline(iss, createdDate, '|');
            
            TaskAssignment* newTask = new TaskAssignment();
            newTask->id = id;
            newTask->supervisorId = supervisorId;
            newTask->staffId = staffId;
            newTask->description = description;
            newTask->taskDetails = taskDetails;
            newTask->location = location;
            newTask->status = status;
            newTask->dueDate = dueDate;
            newTask->createdDate = createdDate;
            newTask->next = head;
            head = newTask;
            
            if (id >= nextId) nextId = id + 1;
        }
    }
    file.close();
}

void TaskAssignmentList::saveToFile() {
    ofstream file(filename);
    if (!file.is_open()) {
        return;
    }

    TaskAssignment* current = head;
    while (current != nullptr) {
        file << current->id << "|"
             << current->supervisorId << "|"
             << current->staffId << "|"
             << current->description << "|"
             << current->taskDetails << "|"
             << current->location << "|"
             << current->status << "|"
             << current->dueDate << "|"
             << current->createdDate << endl;
        current = current->next;
    }
    file.close();
}

int TaskAssignmentList::createTask(int supervisorId, int staffId, string description, string taskDetails, string location, string dueDate) {
    TaskAssignment* newTask = new TaskAssignment();
    newTask->id = nextId++;
    newTask->supervisorId = supervisorId;
    newTask->staffId = staffId;
    newTask->description = description;
    newTask->taskDetails = taskDetails;
    newTask->location = location;
    newTask->status = "pending";
    newTask->dueDate = dueDate;
    newTask->createdDate = "2025-12-10";
    newTask->next = head;
    head = newTask;
    saveToFile();
    return newTask->id;
}

TaskAssignment* TaskAssignmentList::findTaskById(int id) {
    TaskAssignment* current = head;
    while (current != nullptr) {
        if (current->id == id) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

TaskAssignment* TaskAssignmentList::findTasksByStaffId(int staffId) {
    TaskAssignment* current = head;
    while (current != nullptr) {
        if (current->staffId == staffId) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

TaskAssignment* TaskAssignmentList::findTasksBySupervisorId(int supervisorId) {
    TaskAssignment* current = head;
    while (current != nullptr) {
        if (current->supervisorId == supervisorId) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

bool TaskAssignmentList::updateTaskStatus(int taskId, string status) {
    TaskAssignment* task = findTaskById(taskId);
    if (task == nullptr) return false;
    
    task->status = status;
    saveToFile();
    return true;
}

bool TaskAssignmentList::deleteTask(int taskId) {
    if (head == nullptr) return false;
    
    if (head->id == taskId) {
        TaskAssignment* temp = head;
        head = head->next;
        delete temp;
        saveToFile();
        return true;
    }
    
    TaskAssignment* current = head;
    while (current->next != nullptr) {
        if (current->next->id == taskId) {
            TaskAssignment* temp = current->next;
            current->next = current->next->next;
            delete temp;
            saveToFile();
            return true;
        }
        current = current->next;
    }
    return false;
}

string TaskAssignmentList::getTaskJSON(TaskAssignment* task) {
    if (task == nullptr) return "";
    
    string json = "{";
    json += "\"id\":" + to_string(task->id) + ",";
    json += "\"supervisorId\":" + to_string(task->supervisorId) + ",";
    json += "\"staffId\":" + to_string(task->staffId) + ",";
    json += "\"description\":\"" + escapeJSON(task->description) + "\",";
    json += "\"taskDetails\":\"" + escapeJSON(task->taskDetails) + "\",";
    json += "\"location\":\"" + escapeJSON(task->location) + "\",";
    json += "\"status\":\"" + escapeJSON(task->status) + "\",";
    json += "\"dueDate\":\"" + escapeJSON(task->dueDate) + "\",";
    json += "\"createdDate\":\"" + escapeJSON(task->createdDate) + "\"";
    json += "}";
    return json;
}

string TaskAssignmentList::getAllTasksJSON() {
    string json = "[";
    TaskAssignment* current = head;
    bool first = true;
    
    while (current != nullptr) {
        if (!first) json += ",";
        json += getTaskJSON(current);
        first = false;
        current = current->next;
    }
    json += "]";
    return json;
}

string TaskAssignmentList::getStaffTasksJSON(int staffId) {
    string json = "[";
    TaskAssignment* current = head;
    bool first = true;
    
    while (current != nullptr) {
        if (current->staffId == staffId) {
            if (!first) json += ",";
            json += getTaskJSON(current);
            first = false;
        }
        current = current->next;
    }
    json += "]";
    return json;
}

int TaskAssignmentList::getNextTaskId() {
    return nextId;
}

