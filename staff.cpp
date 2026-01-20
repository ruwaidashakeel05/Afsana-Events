#include "staff.h"
#include <sstream>
#include <cstdlib>

StaffList::StaffList() {
    head = nullptr;
    filename = "";
}

StaffList::StaffList(string file) : head(nullptr), filename(file) {
    loadFromFile();
}

StaffList::~StaffList() {
    saveToFile();
    StaffNode* temp;
    while (head) {
        temp = head;
        head = head->next;
        delete temp;
    }
}

void StaffList::addStaff(int id, string name, string role, string email, string password, string phone, string status) {
    StaffNode* newNode = new StaffNode{id, name, role, email, password, phone, nullptr};
    if (!head) {
        head = newNode;
    } else {
        StaffNode* temp = head;
        while (temp->next) temp = temp->next;
        temp->next = newNode;
    }
}

void StaffList::editStaff(int id) {
    StaffNode* staff = head;
    while (staff) {
        if (staff->id == id) {
            cout << "Editing Staff ID: " << id << endl;
            cout << "Enter new name: "; cin >> staff->name;
            cout << "Enter new role: "; cin >> staff->role;
            cout << "Enter new email: "; cin >> staff->email;
            cout << "Enter new phone: "; cin >> staff->phone;
            return;
        }
        staff = staff->next;
    }
    cout << "Staff not found!" << endl;
}

void StaffList::displayStaff() {
    StaffNode* temp = head;
    cout << "\n--- Staff List ---\n";
    while (temp) {
        cout << "ID: " << temp->id
             << ", Name: " << temp->name
             << ", Role: " << temp->role
             << ", Email: " << temp->email
             << ", Phone: " << temp->phone << endl;
        temp = temp->next;
    }
}

StaffNode* StaffList::getStaff(int id) {
    StaffNode* temp = head;
    while (temp) {
        if (temp->id == id) return temp;
        temp = temp->next;
    }
    return nullptr;
}

StaffNode* StaffList::loginStaff(string email, string password) {
    StaffNode* temp = head;
    while (temp) {
        if (temp->email == email && temp->password == password) return temp;
        temp = temp->next;
    }
    return nullptr;
}

void StaffList::saveToFile() {
    ofstream file("staff.txt");
    StaffNode* temp = head;
    while (temp) {
        file << temp->id << "|" << temp->name << "|" << temp->role << "|" << temp->email << "|"
             << temp->password << "|" << temp->phone << endl;
        temp = temp->next;
    }
    file.close();
}

void StaffList::loadFromFile() {
    ifstream file("staff.txt");
    if (!file) return;

    int id;
    string name, role, email, password, phone, line;
    while (getline(file, line)) {
        try {
            stringstream ss(line);
            getline(ss, line, '|'); id = stoi(line);
            getline(ss, name, '|');
            getline(ss, role, '|');
            getline(ss, email, '|');
            getline(ss, password, '|');
            getline(ss, phone, '|');
            addStaff(id, name, role, email, password, phone, "");
        } catch (...) {
            continue;
        }
    }
    file.close();
}

bool StaffList::deleteStaff(int id) {
    if (!head) return false;
    
    // If head is the target
    if (head->id == id) {
        StaffNode* temp = head;
        head = head->next;
        delete temp;
        return true;
    }
    
    // Search for the staff to delete
    StaffNode* current = head;
    while (current->next) {
        if (current->next->id == id) {
            StaffNode* temp = current->next;
            current->next = current->next->next;
            delete temp;
            return true;
        }
        current = current->next;
    }
    
    return false;
}