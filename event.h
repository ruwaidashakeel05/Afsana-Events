#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

// ==================== EVENT NODE STRUCTURE ====================
struct EventNode {
    int id;
    int customerId;  // Added customer ID
    string name;
    string type;
    string date;
    string time;
    string location;
    string address;
    int guests;
    string description;
    string status;
    EventNode* next;

    EventNode(int _id, int _customerId, string _name, string _type, string _date, string _time, 
              string _location, string _address, int _guests, string _desc, string _status = "pending")
        : id(_id), customerId(_customerId), name(_name), type(_type), date(_date), time(_time), 
          location(_location), address(_address), guests(_guests), 
          description(_desc), status(_status), next(nullptr) {}
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
                 string location, string address, int guests, string description);
    EventNode* getEvent(int id);
    bool updateEvent(int id, string name, string type, string date, string time,
                     string location, string address, int guests, string description);
    bool deleteEvent(int id);

    string getAllEventsJSON();
    string getCustomerEventsJSON(int customerId); // NEW

    EventNode* getHead() { return head; }

private:
    void saveToFile();
    void loadFromFile();
};

#endif
