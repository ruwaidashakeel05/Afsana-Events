#include "event.h"

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
             << current->status << endl;
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
        string idStr, customerIdStr, name, type, date, time, location, address, guestsStr, description, status;

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
        getline(ss, status, '|');

        int id = stoi(idStr);
        int customerId = stoi(customerIdStr);
        int guests = stoi(guestsStr);

        EventNode* newEvent = new EventNode(id, customerId, name, type, date, time,
                                            location, address, guests, description, status);

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
                        string location, string address, int guests, string description) {
    EventNode* newEvent = new EventNode(nextId++, customerId, name, type, date, time,
                                        location, address, guests, description);

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
                            string location, string address, int guests, string description) {
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
        json += "\"name\":\"" + current->name + "\",";
        json += "\"type\":\"" + current->type + "\",";
        json += "\"date\":\"" + current->date + "\",";
        json += "\"time\":\"" + current->time + "\",";
        json += "\"location\":\"" + current->location + "\",";
        json += "\"address\":\"" + current->address + "\",";
        json += "\"guests\":" + to_string(current->guests) + ",";
        json += "\"description\":\"" + current->description + "\",";
        json += "\"status\":\"" + current->status + "\"}";
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
            json += "\"name\":\"" + current->name + "\",";
            json += "\"type\":\"" + current->type + "\",";
            json += "\"date\":\"" + current->date + "\",";
            json += "\"time\":\"" + current->time + "\",";
            json += "\"location\":\"" + current->location + "\",";
            json += "\"address\":\"" + current->address + "\",";
            json += "\"guests\":" + to_string(current->guests) + ",";
            json += "\"description\":\"" + current->description + "\",";
            json += "\"status\":\"" + current->status + "\"}";
            first = false;
        }
        current = current->next;
    }

    json += "]";
    return json;
}
