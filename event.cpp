#include "event.h"
#include "customer.h"
#include "supervisor.h"

// ==================== CONSTRUCTOR & DESTRUCTOR ====================
EventList::EventList(string file) : head(nullptr), nextId(1), filename(file) {
    loadFromFile();
}

EventList::~EventList() {
    saveToFile();
    EventNode* current = head;
    while (current != nullptr) {
        EventNode* temp = current;
        current = current->next;
        delete temp;
    }
}

// ==================== FILE HANDLING ====================
void EventList::saveToFile() {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file for writing: " << filename << endl;
        return;
    }

    EventNode* current = head;
    while (current != nullptr) {
        file << current->id << "|" 
             << current->customerId << "|"
             << current->name << "|" 
             << current->type << "|" 
             << current->date << "|"
             << current->time << "|" 
             << current->location << "|" 
             << current->address << "|"
             << current->guests << "|" 
             << current->description << "|" 
             << current->services << "|"
             << current->status << "|"
             << current->assignedSupervisorId << endl;
        current = current->next;
    }
    file.close();
}

void EventList::loadFromFile() {
    ifstream file(filename);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string idStr, customerIdStr, name, type, date, time, location, address, guestsStr, description, services, status, supervisorIdStr;

        getline(ss, idStr, '|');
        getline(ss, customerIdStr, '|');
        getline(ss, name, '|');
        getline(ss, type, '|');
        getline(ss, date, '|');
        getline(ss, time, '|');
        getline(ss, location, '|');
        getline(ss, address, '|');
        getline(ss, guestsStr, '|');
        getline(ss, description, '|');
        getline(ss, services, '|');
        getline(ss, status, '|');
        getline(ss, supervisorIdStr, '|');

        int id = 0, customerId = 0, guests = 0, supervisorId = 0;
        try {
            id = stoi(idStr);
            customerId = stoi(customerIdStr);
            guests = stoi(guestsStr);
            supervisorId = stoi(supervisorIdStr);
        } catch (...) {
            continue;
        }

        EventNode* newEvent = new EventNode(id, customerId, name, type, date, time,
                                            location, address, guests, description, services, status);
        newEvent->assignedSupervisorId = supervisorId;

        if (head == nullptr) {
            head = newEvent;
        } else {
            EventNode* current = head;
            while (current->next != nullptr)
                current = current->next;
            current->next = newEvent;
        }

        if (id >= nextId) nextId = id + 1;
    }
    file.close();
}

// ==================== EVENT OPERATIONS ====================
int EventList::addEvent(int customerId, string name, string type, string date, string time,
                        string location, string address, int guests, string description, string services) {
    EventNode* newEvent = new EventNode(nextId++, customerId, name, type, date, time,
                                        location, address, guests, description, services);

    if (head == nullptr) head = newEvent;
    else {
        EventNode* current = head;
        while (current->next != nullptr) current = current->next;
        current->next = newEvent;
    }
    saveToFile();
    return newEvent->id;
}

EventNode* EventList::getEvent(int id) {
    EventNode* current = head;
    while (current != nullptr) {
        if (current->id == id) return current;
        current = current->next;
    }
    return nullptr;
}

bool EventList::updateEvent(int id, string name, string type, string date, string time,
                            string location, string address, int guests, string description, string services) {
    EventNode* event = getEvent(id);
    if (event != nullptr) {
        event->name = name;
        event->type = type;
        event->date = date;
        event->time = time;
        event->location = location;
        event->address = address;
        event->guests = guests;
        event->description = description;
        event->services = services;
        saveToFile();
        return true;
    }
    return false;
}

bool EventList::deleteEvent(int id) {
    if (head == nullptr) return false;

    if (head->id == id) {
        EventNode* temp = head;
        head = head->next;
        delete temp;
        saveToFile();
        return true;
    }

    EventNode* current = head;
    while (current->next != nullptr) {
        if (current->next->id == id) {
            EventNode* temp = current->next;
            current->next = current->next->next;
            delete temp;
            saveToFile();
            return true;
        }
        current = current->next;
    }
    return false;
}

// ==================== JSON OUTPUT ====================
string EventList::getAllEventsJSON() {
    string json = "[";
    EventNode* current = head;
    bool first = true;

    while (current != nullptr) {
        if (!first) json += ",";
        json += "{\"id\":" + to_string(current->id) + ",";
        json += "\"name\":\"" + escapeJSON(current->name) + "\",";
        json += "\"type\":\"" + escapeJSON(current->type) + "\",";
        json += "\"date\":\"" + escapeJSON(current->date) + "\",";
        json += "\"time\":\"" + escapeJSON(current->time) + "\",";
        json += "\"location\":\"" + escapeJSON(current->location) + "\",";
        json += "\"address\":\"" + escapeJSON(current->address) + "\",";
        json += "\"guests\":" + to_string(current->guests) + ",";
        json += "\"description\":\"" + escapeJSON(current->description) + "\",";
        json += "\"services\":\"" + escapeJSON(current->services) + "\",";
        json += "\"status\":\"" + escapeJSON(current->status) + "\"}";
        first = false;
        current = current->next;
    }

    json += "]";
    return json;
}

// ==================== JSON FOR CUSTOMER EVENTS ====================
string EventList::getCustomerEventsJSON(int customerId) {
    string json = "[";
    EventNode* current = head;
    bool first = true;

    while (current != nullptr) {
        if (current->customerId == customerId) {
            if (!first) json += ",";
            json += "{\"id\":" + to_string(current->id) + ",";
            json += "\"name\":\"" + escapeJSON(current->name) + "\",";
            json += "\"type\":\"" + escapeJSON(current->type) + "\",";
            json += "\"date\":\"" + escapeJSON(current->date) + "\",";
            json += "\"time\":\"" + escapeJSON(current->time) + "\",";
            json += "\"location\":\"" + escapeJSON(current->location) + "\",";
            json += "\"address\":\"" + escapeJSON(current->address) + "\",";
            json += "\"guests\":" + to_string(current->guests) + ",";
            json += "\"description\":\"" + escapeJSON(current->description) + "\",";
            json += "\"services\":\"" + escapeJSON(current->services) + "\",";
            json += "\"status\":\"" + escapeJSON(current->status) + "\"}";
            first = false;
        }
        current = current->next;
    }

    json += "]";
    return json;
}

string EventList::getCustomerEventsWithSupervisorJSON(int customerId, SupervisorList* supervisorList) {
    string json = "[";
    EventNode* current = head;
    bool first = true;

    while (current != nullptr) {
        if (current->customerId == customerId) {
            if (!first) json += ",";
            json += "{\"id\":" + to_string(current->id) + ",";
            json += "\"name\":\"" + escapeJSON(current->name) + "\",";
            json += "\"type\":\"" + escapeJSON(current->type) + "\",";
            json += "\"date\":\"" + escapeJSON(current->date) + "\",";
            json += "\"time\":\"" + escapeJSON(current->time) + "\",";
            json += "\"location\":\"" + escapeJSON(current->location) + "\",";
            json += "\"address\":\"" + escapeJSON(current->address) + "\",";
            json += "\"guests\":" + to_string(current->guests) + ",";
            json += "\"description\":\"" + escapeJSON(current->description) + "\",";
            json += "\"services\":\"" + escapeJSON(current->services) + "\",";
            json += "\"status\":\"" + escapeJSON(current->status) + "\"";
            
            // Add supervisor info if event is confirmed and has assigned supervisor
            if (current->status == "confirmed" && current->assignedSupervisorId != 0 && supervisorList) {
                SupervisorNode* supervisor = supervisorList->findById(current->assignedSupervisorId);
                if (supervisor) {
                    json += ",\"supervisor\":{\"id\":" + to_string(supervisor->id) + 
                            ",\"name\":\"" + escapeJSON(supervisor->name) +
                            "\",\"email\":\"" + escapeJSON(supervisor->email) +
                            "\",\"phone\":\"" + escapeJSON(supervisor->phone) + "\"}";
                }
            }
            
            json += "}";
            first = false;
        }
        current = current->next;
    }

    json += "]";
    return json;
}

string EventList::getAllEventsWithCustomerJSON(CustomerList* customerList) {
    string json = "[";
    EventNode* current = head;
    bool first = true;

    while (current != nullptr) {
        if (!first) json += ",";
        
        // Get customer name from customer list
        CustomerNode* customer = customerList->getHead();
        string customerName = "Unknown";
        while (customer != nullptr) {
            if (customer->id == current->customerId) {
                customerName = customer->name;
                break;
            }
            customer = customer->next;
        }
        
        json += "{\"id\":" + to_string(current->id) + ",";
        json += "\"customerId\":" + to_string(current->customerId) + ",";
        json += "\"name\":\"" + escapeJSON(current->name) + "\",";
        json += "\"customerName\":\"" + escapeJSON(customerName) + "\",";
        json += "\"type\":\"" + escapeJSON(current->type) + "\",";
        json += "\"date\":\"" + escapeJSON(current->date) + "\",";
        json += "\"time\":\"" + escapeJSON(current->time) + "\",";
        json += "\"location\":\"" + escapeJSON(current->location) + "\",";
        json += "\"address\":\"" + escapeJSON(current->address) + "\",";
        json += "\"guests\":" + to_string(current->guests) + ",";
        json += "\"description\":\"" + escapeJSON(current->description) + "\",";
        json += "\"services\":\"" + escapeJSON(current->services) + "\",";
        json += "\"status\":\"" + escapeJSON(current->status) + "\"}";
        first = false;
        current = current->next;
    }

    json += "]";
    return json;
}

string EventList::getPendingEventsJSON() {
    string json = "[";
    bool first = true;
    EventNode* current = head;
    
    while (current) {
        if (current->status == "pending") {
            if (!first) json += ",";
            json += "{\"id\":" + to_string(current->id) +
                    ",\"customerId\":" + to_string(current->customerId) +
                    ",\"name\":\"" + escapeJSON(current->name) +
                    "\",\"type\":\"" + escapeJSON(current->type) +
                    "\",\"date\":\"" + current->date +
                    "\",\"time\":\"" + current->time +
                    "\",\"location\":\"" + escapeJSON(current->location) +
                    "\",\"address\":\"" + escapeJSON(current->address) +
                    "\",\"guests\":" + to_string(current->guests) +
                    ",\"description\":\"" + escapeJSON(current->description) +
                    "\",\"services\":\"" + escapeJSON(current->services) +
                    "\",\"status\":\"" + current->status + "\"}";
            first = false;
        }
        current = current->next;
    }
    json += "]";
    return json;
}

string EventList::getSupervisorEventsJSON(int supervisorId) {
    string json = "[";
    bool first = true;
    EventNode* current = head;
    
    while (current) {
        if (current->assignedSupervisorId == supervisorId) {
            if (!first) json += ",";
            json += "{\"id\":" + to_string(current->id) +
                    ",\"customerId\":" + to_string(current->customerId) +
                    ",\"name\":\"" + escapeJSON(current->name) +
                    "\",\"type\":\"" + escapeJSON(current->type) +
                    "\",\"date\":\"" + current->date +
                    "\",\"time\":\"" + current->time +
                    "\",\"location\":\"" + escapeJSON(current->location) +
                    "\",\"address\":\"" + escapeJSON(current->address) +
                    "\",\"guests\":" + to_string(current->guests) +
                    ",\"description\":\"" + escapeJSON(current->description) +
                    "\",\"services\":\"" + escapeJSON(current->services) +
                    "\",\"status\":\"" + current->status +
                    "\",\"assignedSupervisorId\":" + to_string(current->assignedSupervisorId) + "}";
            first = false;
        }
        current = current->next;
    }
    json += "]";
    return json;
}

bool EventList::assignEventToSupervisor(int eventId, int supervisorId) {
    EventNode* event = getEvent(eventId);
    if (event) {
        event->assignedSupervisorId = supervisorId;
        saveToFile();
        return true;
    }
    return false;
}

bool EventList::updateEventStatus(int eventId, string newStatus) {
    EventNode* event = getEvent(eventId);
    if (event) {
        event->status = newStatus;
        saveToFile();
        return true;
    }
    return false;
}