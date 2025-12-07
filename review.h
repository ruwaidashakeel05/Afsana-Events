#ifndef REVIEW_H
#define REVIEW_H

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

struct ReviewNode {
    int id;
    int staffId;
    int customerId;
    int rating;
    string comment;
    string date;
    ReviewNode* next;
};

class ReviewList {
private:
    ReviewNode* head;
    int nextId;

public:
    ReviewList() : head(nullptr), nextId(1) {}
    
    ~ReviewList() {
        while (head) {
            ReviewNode* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void addReview(int staffId, int rating, string comment) {
        ReviewNode* newNode = new ReviewNode();
        newNode->id = nextId++;
        newNode->staffId = staffId;
        newNode->rating = rating;
        newNode->comment = comment;
        newNode->date = "";
        newNode->next = head;
        head = newNode;
    }

    string getAllReviewsJSON() {
        string json = "[";
        bool first = true;
        ReviewNode* current = head;
        
        while (current) {
            if (!first) json += ",";
            json += "{\"id\":" + to_string(current->id) + 
                    ",\"staffId\":" + to_string(current->staffId) +
                    ",\"rating\":" + to_string(current->rating) + 
                    ",\"comment\":\"" + current->comment + "\"}";
            first = false;
            current = current->next;
        }
        json += "]";
        return json;
    }
};

// Keep the old ReviewStack for backward compatibility
struct ReviewStackNode {
    int id;
    string staffName;
    string role;
    int rating;
    string comment;
    string customer;
    string date;
    ReviewStackNode* next;
};

class ReviewStack {
private:
    ReviewStackNode* top;

public:
    ReviewStack();
    ~ReviewStack();

    void pushReview(int id, string staffName, string role, int rating, string comment, string customer, string date);
    void displayReviews();

    void saveToFile();
    void loadFromFile();
};

#endif

