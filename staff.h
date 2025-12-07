#ifndef STAFF_H
#define STAFF_H

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

struct StaffNode {
    int id;
    string name;
    string role;
    string email;
    string phone;
    string status; // available or busy
    StaffNode* next;
};

class StaffList {
private:
    StaffNode* head;

public:
    StaffList();
    ~StaffList();

    void addStaff(int id, string name, string role, string email, string phone, string status);
    void editStaff(int id);
    void displayStaff();
    StaffNode* getStaff(int id);
    
    string getAllStaffJSON() {
        string json = "[";
        bool first = true;
        StaffNode* current = head;
        
        while (current) {
            if (!first) json += ",";
            json += "{\"id\":" + to_string(current->id) + 
                    ",\"name\":\"" + current->name + 
                    "\",\"role\":\"" + current->role + 
                    "\",\"status\":\"" + current->status + "\"}";
            first = false;
            current = current->next;
        }
        json += "]";
        return json;
    }

    void saveToFile();
    void loadFromFile();
};

#endif
