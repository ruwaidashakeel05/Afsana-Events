#ifndef STAFF_H
#define STAFF_H

#include <iostream>
#include <fstream>
#include <string>
using namespace std;
//link l
struct StaffNode {
    int id;
    string name;
    string role;
    string email;
    string password;
    string phone;
    StaffNode* next;
};

class StaffList {
private:
    StaffNode* head;
    string filename;

public:
    StaffList();
    StaffList(string file);
    ~StaffList();

    void addStaff(int id, string name, string role, string email, string password, string phone, string status);
    void editStaff(int id);
    void displayStaff();
    StaffNode* getStaff(int id);
    StaffNode* loginStaff(string email, string password);
    bool deleteStaff(int id);
    
    string getStaffJSON(StaffNode* staff) {
        if (!staff) return "";
        return "{\"id\":" + to_string(staff->id) + 
               ",\"name\":\"" + staff->name + 
               "\",\"role\":\"" + staff->role + 
               "\",\"email\":\"" + staff->email + 
               "\",\"phone\":\"" + staff->phone + "\"}";
    }
    
    string getAllStaffJSON() {
        string json = "[";
        bool first = true;
        StaffNode* current = head;
        
        while (current) {
            if (!first) json += ",";
            json += "{\"id\":" + to_string(current->id) + 
                    ",\"name\":\"" + current->name + 
                    "\",\"role\":\"" + current->role + 
                    "\",\"email\":\"" + current->email + 
                    "\",\"phone\":\"" + current->phone + "\"}";
            first = false;
            current = current->next;
        }
        json += "]";
        return json;
    }

    int getNextId() {
        int maxId = 0;
        StaffNode* current = head;
        while (current) {
            if (current->id > maxId) maxId = current->id;
            current = current->next;
        }
        return maxId + 1;
    }

    void saveToFile();
    void loadFromFile();
};

#endif
