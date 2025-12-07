#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <iostream>
#include <string>
#include <queue>
using namespace std;

struct NotificationNode {
    int id;
    int customerId;
    string message;
    string timestamp;
    bool read;
    NotificationNode* next;
};

class NotificationQueue {
private:
    NotificationNode* front;
    NotificationNode* rear;
    int nextId;

public:
    NotificationQueue() : front(nullptr), rear(nullptr), nextId(1) {}
    
    ~NotificationQueue() {
        while (front) {
            NotificationNode* temp = front;
            front = front->next;
            delete temp;
        }
    }

    void enqueue(int customerId, string message, string timestamp) {
        NotificationNode* newNode = new NotificationNode();
        newNode->id = nextId++;
        newNode->customerId = customerId;
        newNode->message = message;
        newNode->timestamp = timestamp;
        newNode->read = false;
        newNode->next = nullptr;

        if (!rear) {
            front = rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }
    }

    bool markAsRead(int notificationId) {
        NotificationNode* current = front;
        while (current) {
            if (current->id == notificationId) {
                current->read = true;
                return true;
            }
            current = current->next;
        }
        return false;
    }

    string getCustomerNotificationsJSON(int customerId) {
        string json = "[";
        bool first = true;
        NotificationNode* current = front;
        
        while (current) {
            if (current->customerId == customerId) {
                if (!first) json += ",";
                json += "{\"id\":" + to_string(current->id) + 
                        ",\"message\":\"" + current->message + 
                        "\",\"timestamp\":\"" + current->timestamp + 
                        "\",\"read\":" + (current->read ? "true" : "false") + "}";
                first = false;
            }
            current = current->next;
        }
        json += "]";
        return json;
    }
};

#endif
