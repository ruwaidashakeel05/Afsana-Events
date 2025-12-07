#include "supervisor.h"

void Supervisor::loadAllData() {
    staffList.loadFromFile();
    taskQueue.loadFromFile();
    reviewStack.loadFromFile();
}

void Supervisor::saveAllData() {
    staffList.saveToFile();
    taskQueue.saveToFile();
    reviewStack.saveToFile();
}

void Supervisor::staffManagementMenu() {
    int choice;
    do {
        cout << "\n--- Staff Management ---\n";
        cout << "1. Add Staff\n2. Edit Staff\n3. View Staff\n0. Back\nChoice: ";
        cin >> choice;
        if (choice == 1) {
            int id; string name, role, email, phone, status;
            cout << "Enter ID: "; cin >> id;
            cout << "Enter Name: "; cin >> name;
            cout << "Enter Role: "; cin >> role;
            cout << "Enter Email: "; cin >> email;
            cout << "Enter Phone: "; cin >> phone;
            cout << "Enter Status: "; cin >> status;
            staffList.addStaff(id, name, role, email, phone, status);
        } else if (choice == 2) {
            int id; cout << "Enter Staff ID to edit: "; cin >> id;
            staffList.editStaff(id);
        } else if (choice == 3) {
            staffList.displayStaff();
        }
    } while (choice != 0);
}

void Supervisor::taskAssignmentMenu() {
    int choice;
    do {
        cout << "\n--- Task Assignment ---\n";
        cout << "1. Assign Task\n2. View Tasks\n0. Back\nChoice: ";
        cin >> choice;
        if (choice == 1) {
            int id; string name, eventName, assignedTo, dueDate, description;
            cout << "Enter Task ID: "; cin >> id;
            cout << "Task Name: "; cin >> name;
            cout << "Event Name: "; cin >> eventName;
            cout << "Assign To (Staff Name): "; cin >> assignedTo;
            cout << "Due Date (YYYY-MM-DD): "; cin >> dueDate;
            cout << "Description: "; cin.ignore(); getline(cin, description);
            taskQueue.assignTask(id, name, eventName, assignedTo, dueDate, description);
        } else if (choice == 2) {
            taskQueue.viewTasks();
        }
    } while (choice != 0);
}

void Supervisor::reviewMenu() {
    reviewStack.displayReviews();
}

void Supervisor::mainMenu() {
    loadAllData();
    int choice;
    do {
        cout << "\n--- Supervisor Dashboard ---\n";
        cout << "1. Staff Management\n2. Task Assignment\n3. Staff Reviews\n0. Exit\nChoice: ";
        cin >> choice;
        if (choice == 1) staffManagementMenu();
        else if (choice == 2) taskAssignmentMenu();
        else if (choice == 3) reviewMenu();
    } while (choice != 0);
    saveAllData();
}
