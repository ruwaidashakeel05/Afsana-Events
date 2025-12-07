#include "staff.h"
#include <sstream>
#include <cstdlib>

StaffList::StaffList() {
    head = nullptr;
}

StaffList::~StaffList() {
    StaffNode* temp;
    while (head) {
        temp = head;
        head = head->next;
        delete temp;
    }
}

void StaffList::addStaff(int id, string name, string role, string email, string phone, string status) {
    StaffNode* newNode = new StaffNode{id, name, role, email, phone, status, nullptr};
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
            cout << "Enter new status: "; cin >> staff->status;
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
             << ", Phone: " << temp->phone
             << ", Status: " << temp->status << endl;
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

void StaffList::saveToFile() {
    ofstream file("staff.txt");
    StaffNode* temp = head;
    while (temp) {
        file << temp->id << "," << temp->name << "," << temp->role << "," << temp->email << ","
             << temp->phone << "," << temp->status << endl;
        temp = temp->next;
    }
    file.close();
}

void StaffList::loadFromFile() {
    ifstream file("staff.txt");
    if (!file) return;

    int id;
    string name, role, email, phone, status, line;
    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, line, ','); id = stoi(line);
        getline(ss, name, ',');
        getline(ss, role, ',');
        getline(ss, email, ',');
        getline(ss, phone, ',');
        getline(ss, status, ',');
        addStaff(id, name, role, email, phone, status);
    }
    file.close();
}
