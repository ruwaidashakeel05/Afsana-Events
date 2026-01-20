#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "utils.h"

using namespace std;

// ==================== EVENT NODE STRUCTURE ====================
struct EventNode {
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
    string status;          // pending, confirmed, completed
    int assignedSupervisorId;  // Supervisor ID who accepted the event (0 if not assigned)
    EventNode* next;

    EventNode(int _id, int _customerId, string _name, string _type, string _date, string _time, 
              string _location, string _address, int _guests, string _desc, string _services = "", string _status = "pending")
        : id(_id), customerId(_customerId), name(_name), type(_type), date(_date), time(_time), 
          location(_location), address(_address), guests(_guests), 
          description(_desc), services(_services), status(_status), assignedSupervisorId(0), next(nullptr) {}
};

// ==================== LINKED LIST FOR EVENTS ====================
class EventList {
private:
    EventNode* head;
    int nextId;
    string filename;

public:
    EventList(string file = "events.txt");
    ~EventList();

    int addEvent(int customerId, string name, string type, string date, string time,
                 string location, string address, int guests, string description, string services);
    EventNode* getEvent(int id);
    bool updateEvent(int id, string name, string type, string date, string time,
                     string location, string address, int guests, string description, string services);
    bool deleteEvent(int id);

    string getAllEventsJSON();
    string getCustomerEventsJSON(int customerId); // NEW
    string getCustomerEventsWithSupervisorJSON(int customerId, class SupervisorList* supervisorList);
    string getAllEventsWithCustomerJSON(class CustomerList* customerList);
    
    // Supervisor event management
    string getPendingEventsJSON();
    string getSupervisorEventsJSON(int supervisorId);
    bool assignEventToSupervisor(int eventId, int supervisorId);
    bool updateEventStatus(int eventId, string newStatus);

    EventNode* getHead() { return head; }
    void saveToFile();

private:
    void loadFromFile();
};

#endif
