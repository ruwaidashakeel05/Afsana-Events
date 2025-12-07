#include "review.h"
#include <sstream>

ReviewStack::ReviewStack() {
    top = nullptr;
}

ReviewStack::~ReviewStack() {
    ReviewStackNode* temp;
    while (top) {
        temp = top;
        top = top->next;
        delete temp;
    }
}

void ReviewStack::pushReview(int id, string staffName, string role, int rating, string comment, string customer, string date) {
    ReviewStackNode* newNode = new ReviewStackNode();
    newNode->id = id;
    newNode->staffName = staffName;
    newNode->role = role;
    newNode->rating = rating;
    newNode->comment = comment;
    newNode->customer = customer;
    newNode->date = date;
    newNode->next = top;
    top = newNode;
}

void ReviewStack::displayReviews() {
    ReviewStackNode* temp = top;
    cout << "\n--- Staff Reviews ---\n";
    while (temp) {
        cout << "Staff: " << temp->staffName << ", Role: " << temp->role << ", Rating: " << temp->rating << endl;
        cout << "Comment: " << temp->comment << endl;
        cout << "By: " << temp->customer << ", Date: " << temp->date << endl << endl;
        temp = temp->next;
    }
}

void ReviewStack::saveToFile() {
    ofstream file("reviews.txt");
    ReviewStackNode* temp = top;
    while (temp) {
        file << temp->id << "," << temp->staffName << "," << temp->role << "," << temp->rating << "," 
             << temp->comment << "," << temp->customer << "," << temp->date << endl;
        temp = temp->next;
    }
    file.close();
}

void ReviewStack::loadFromFile() {
    ifstream file("reviews.txt");
    if (!file) return;

    string line;
    int id, rating;
    string staffName, role, comment, customer, date;

    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, line, ','); id = stoi(line);
        getline(ss, staffName, ',');
        getline(ss, role, ',');
        getline(ss, line, ','); rating = stoi(line);
        getline(ss, comment, ',');
        getline(ss, customer, ',');
        getline(ss, date);
        pushReview(id, staffName, role, rating, comment, customer, date);
    }
    file.close();
}
