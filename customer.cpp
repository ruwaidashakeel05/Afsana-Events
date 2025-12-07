#include "customer.h"

// ==================== CONSTRUCTOR & DESTRUCTOR ====================
CustomerList::CustomerList(string file) : head(nullptr), nextId(1), filename(file) {
    loadFromFile();
}

CustomerList::~CustomerList() {
    saveToFile();
    CustomerNode* current = head;
    while (current != nullptr) {
        CustomerNode* temp = current;
        current = current->next;
        delete temp;
    }
}

// ==================== FILE HANDLING ====================
void CustomerList::saveToFile() {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file for writing: " << filename << endl;
        return;
    }

    CustomerNode* current = head;
    while (current != nullptr) {
        file << current->id << "|" 
             << current->name << "|" 
             << current->email << "|" 
             << current->password << "|" 
             << current->phone << "|" 
             << current->address << endl;
        current = current->next;
    }
    file.close();
}

void CustomerList::loadFromFile() {
    ifstream file(filename);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string idStr, name, email, password, phone, address;

        getline(ss, idStr, '|');
        getline(ss, name, '|');
        getline(ss, email, '|');
        getline(ss, password, '|');
        getline(ss, phone, '|');
        getline(ss, address, '|');

        int id = stoi(idStr);
        CustomerNode* newCustomer = new CustomerNode(id, name, email, password, phone, address);

        if (head == nullptr) head = newCustomer;
        else {
            CustomerNode* current = head;
            while (current->next != nullptr) current = current->next;
            current->next = newCustomer;
        }

        if (id >= nextId) nextId = id + 1;
    }
    file.close();
}

// ==================== CUSTOMER OPERATIONS ====================
int CustomerList::registerCustomer(string name, string email, string password, string phone, string address) {
    // Check if email already exists
    CustomerNode* current = head;
    while (current != nullptr) {
        if (current->email == email) return -1;
        current = current->next;
    }

    CustomerNode* newCustomer = new CustomerNode(nextId++, name, email, password, phone, address);
    if (head == nullptr) head = newCustomer;
    else {
        CustomerNode* current = head;
        while (current->next != nullptr) current = current->next;
        current->next = newCustomer;
    }
    saveToFile();
    return newCustomer->id;
}

CustomerNode* CustomerList::login(string email, string password) {
    CustomerNode* current = head;
    while (current != nullptr) {
        if (current->email == email && current->password == password)
            return current;
        current = current->next;
    }
    return nullptr;
}

CustomerNode* CustomerList::findById(int id) {
    CustomerNode* current = head;
    while (current != nullptr) {
        if (current->id == id) return current;
        current = current->next;
    }
    return nullptr;
}

bool CustomerList::updateCustomer(int id, string name, string phone, string address) {
    CustomerNode* customer = findById(id);
    if (customer != nullptr) {
        customer->name = name;
        customer->phone = phone;
        customer->address = address;
        saveToFile();
        return true;
    }
    return false;
}

// ==================== JSON OUTPUT ====================
string CustomerList::getCustomerJSON(CustomerNode* customer) {
    if (customer == nullptr) return "{}";

    string json = "{";
    json += "\"id\":" + to_string(customer->id) + ",";
    json += "\"name\":\"" + customer->name + "\",";
    json += "\"email\":\"" + customer->email + "\",";
    json += "\"phone\":\"" + customer->phone + "\",";
    json += "\"address\":\"" + customer->address + "\"";
    json += "}";

    return json;
}
