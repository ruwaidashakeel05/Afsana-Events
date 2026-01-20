#ifndef EVENTSTACK_H
#define EVENTSTACK_H

#include "event.h"
#include "utils.h"
#include <iostream>
#include <string>
using namespace std;

struct HistoryNode {
    int id;
    int customerId;
    string name;
    string type;
    string date;
    string time;
    string location;
    string address;
    int guests;
    string description;
    string services;
    string status;
    string completedDate;
    HistoryNode* next;
};

class EventStack {
private:
    HistoryNode* top;

public:
    EventStack() : top(nullptr) {}
    
    ~EventStack() {
        while (top) {
            HistoryNode* temp = top;
            top = top->next;
            delete temp;
        }
    }

    void push(EventNode* event) {
        if (!event) return;
        
        HistoryNode* newNode = new HistoryNode();
        newNode->id = event->id;
        newNode->customerId = event->customerId;
        newNode->name = event->name;
        newNode->type = event->type;
        newNode->date = event->date;
        newNode->time = event->time;
        newNode->location = event->location;
        newNode->address = event->address;
        newNode->guests = event->guests;
        newNode->description = event->description;
        newNode->services = event->services;
        newNode->status = event->status;
        newNode->completedDate = "";
        newNode->next = top;
        top = newNode;
    }

    string getAllHistoryJSON() {
        string json = "[";
        bool first = true;
        HistoryNode* current = top;
        
        while (current) {
            if (!first) json += ",";
            json += "{\"id\":" + to_string(current->id) + 
                    ",\"customerId\":" + to_string(current->customerId) +
                    ",\"name\":\"" + current->name + 
                    "\",\"type\":\"" + current->type + 
                    "\",\"date\":\"" + current->date + 
                    "\",\"services\":\"" + current->services + 
                    "\",\"status\":\"" + current->status + "\"}";
            first = false;
            current = current->next;
        }
        json += "]";
        return json;
    }

    string getCustomerHistoryJSON(int customerId) {
        string json = "[";
        bool first = true;
        HistoryNode* current = top;
        
        while (current) {
            if (current->customerId == customerId) {
                if (!first) json += ",";
                json += "{\"id\":" + to_string(current->id) + 
                        ",\"customerId\":" + to_string(current->customerId) +
                        ",\"name\":\"" + escapeJSON(current->name) + 
                        "\",\"type\":\"" + escapeJSON(current->type) + 
                        "\",\"date\":\"" + escapeJSON(current->date) + 
                        "\",\"time\":\"" + escapeJSON(current->time) +
                        "\",\"location\":\"" + escapeJSON(current->location) +
                        "\",\"address\":\"" + escapeJSON(current->address) +
                        "\",\"guests\":" + to_string(current->guests) +
                        ",\"description\":\"" + escapeJSON(current->description) +
                        "\",\"services\":\"" + escapeJSON(current->services) + 
                        "\",\"status\":\"" + escapeJSON(current->status) + "\"}";
                first = false;
            }
            current = current->next;
        }
        json += "]";
        return json;
    }
};

#endif
