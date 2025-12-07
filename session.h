#ifndef SESSION_H
#define SESSION_H

#include <iostream>
#include <string>
using namespace std;

struct SessionNode {
    int customerId;
    string token;
    string loginTime;
    SessionNode* next;
};

class SessionStack {
private:
    SessionNode* top;

public:
    SessionStack() : top(nullptr) {}
    
    ~SessionStack() {
        while (top) {
            SessionNode* temp = top;
            top = top->next;
            delete temp;
        }
    }

    void push(int customerId, string token, string loginTime) {
        SessionNode* newNode = new SessionNode();
        newNode->customerId = customerId;
        newNode->token = token;
        newNode->loginTime = loginTime;
        newNode->next = top;
        top = newNode;
    }

    bool removeSession(string token) {
        if (!top) return false;

        if (top->token == token) {
            SessionNode* temp = top;
            top = top->next;
            delete temp;
            return true;
        }

        SessionNode* current = top;
        while (current->next) {
            if (current->next->token == token) {
                SessionNode* temp = current->next;
                current->next = temp->next;
                delete temp;
                return true;
            }
            current = current->next;
        }
        return false;
    }

    int findCustomerByToken(string token) {
        SessionNode* current = top;
        while (current) {
            if (current->token == token) {
                return current->customerId;
            }
            current = current->next;
        }
        return -1;
    }
};

#endif
