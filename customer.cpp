#include "customer.h"
#include "utils.h"

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
        try {
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
        } catch (...) {
            continue;
        }
    }
    file.close();
}

// ==================== CUSTOMER OPERATIONS ====================

// Function to register a new customer with full validation
int CustomerList::registerCustomer(string name, string email, string password, string phone, string address) {
    // ===== VALIDATION CHECKS =====
    
    // Check if name is empty or too short
    if (name.empty() || name.length() < 2) {
        cerr << "Error: Name must be at least 2 characters" << endl;
        return -1;
    }
    
    // Check if name is too long (max 100 characters)
    if (name.length() > 100) {
        cerr << "Error: Name cannot exceed 100 characters" << endl;
        return -1;
    }
    
    // Check if email is empty
    if (email.empty()) {
        cerr << "Error: Email cannot be empty" << endl;
        return -1;
    }
    
    // Check if email has @ symbol and . (basic validation)
    if (email.find('@') == string::npos || email.find('.') == string::npos) {
        cerr << "Error: Invalid email format" << endl;
        return -1;
    }
    
    // Check if password is empty
    if (password.empty()) {
        cerr << "Error: Password cannot be empty" << endl;
        return -1;
    }
    
    // Check if password is strong enough (at least 6 characters)
    if (password.length() < 6) {
        cerr << "Error: Password must be at least 6 characters" << endl;
        return -1;
    }
    
    // Check if phone is empty
    if (phone.empty()) {
        cerr << "Error: Phone cannot be empty" << endl;
        return -1;
    }
    
    // Check if phone has exactly 11 digits
    if (phone.length() != 11) {
        cerr << "Error: Phone must be exactly 11 digits" << endl;
        return -1;
    }
    
    // Check if all phone characters are digits
    for (char c : phone) {
        if (!isdigit(c)) {
            cerr << "Error: Phone must contain only digits" << endl;
            return -1;
        }
    }
    
    // Check if phone starts with 03 (Pakistan number)
    if (phone[0] != '0' || phone[1] != '3') {
        cerr << "Error: Phone must start with 03 (Pakistan format)" << endl;
        return -1;
    }
    
    // Check if address is empty
    if (address.empty()) {
        cerr << "Error: Address cannot be empty" << endl;
        return -1;
    }
    
    // Check if address is too short
    if (address.length() < 5) {
        cerr << "Error: Address must be at least 5 characters" << endl;
        return -1;
    }
    
    // Check if address is too long
    if (address.length() > 200) {
        cerr << "Error: Address cannot exceed 200 characters" << endl;
        return -1;
    }
    
    // ===== EMAIL DUPLICATE CHECK =====
    // Check if email already exists in the system
    CustomerNode* current = head;
    while (current != nullptr) {
        if (current->email == email) {
            cerr << "Error: Email already registered" << endl;
            return -1;  // Email already exists
        }
        current = current->next;
    }

    // ===== CREATE NEW CUSTOMER =====
    // Create new customer node with validated data
    CustomerNode* newCustomer = new CustomerNode(nextId++, name, email, password, phone, address);
    
    // Add to beginning if list is empty
    if (head == nullptr) {
        head = newCustomer;
    } else {
        // Add to end of list
        CustomerNode* current = head;
        while (current->next != nullptr) current = current->next;
        current->next = newCustomer;
    }
    
    // Save to file
    saveToFile();
    cout << "Customer registered successfully with ID: " << newCustomer->id << endl;
    return newCustomer->id;  // Return new customer ID
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
    // ===== VALIDATION CHECKS =====
    
    // Check if name is empty or too short
    if (name.empty() || name.length() < 2) {
        cerr << "Error: Name must be at least 2 characters" << endl;
        return false;
    }
    
    // Check if name is too long (max 100 characters)
    if (name.length() > 100) {
        cerr << "Error: Name cannot exceed 100 characters" << endl;
        return false;
    }
    
    // Check if phone is empty
    if (phone.empty()) {
        cerr << "Error: Phone cannot be empty" << endl;
        return false;
    }
    
    // Check if phone has exactly 11 digits
    if (phone.length() != 11) {
        cerr << "Error: Phone must be exactly 11 digits" << endl;
        return false;
    }
    
    // Check if all phone characters are digits
    for (char c : phone) {
        if (!isdigit(c)) {
            cerr << "Error: Phone must contain only digits" << endl;
            return false;
        }
    }
    
    // Check if phone starts with 03
    if (phone[0] != '0' || phone[1] != '3') {
        cerr << "Error: Phone must start with 03" << endl;
        return false;
    }
    
    // Check if address is empty
    if (address.empty()) {
        cerr << "Error: Address cannot be empty" << endl;
        return false;
    }
    
    // Check if address is too short
    if (address.length() < 5) {
        cerr << "Error: Address must be at least 5 characters" << endl;
        return false;
    }
    
    // Check if address is too long
    if (address.length() > 200) {
        cerr << "Error: Address cannot exceed 200 characters" << endl;
        return false;
    }
    
    // ===== UPDATE CUSTOMER =====
    // Find customer by ID and update with validated data
    CustomerNode* customer = findById(id);
    if (customer != nullptr) {
        customer->name = name;
        customer->phone = phone;
        customer->address = address;
        saveToFile();
        cout << "Customer updated successfully" << endl;
        return true;
    }
    cerr << "Error: Customer not found" << endl;
    return false;
}

bool CustomerList::deleteCustomer(int id) {
    if (head == nullptr) return false;
    
    if (head->id == id) {
        CustomerNode* temp = head;
        head = head->next;
        delete temp;
        saveToFile();
        return true;
    }
    
    CustomerNode* current = head;
    while (current->next != nullptr) {
        if (current->next->id == id) {
            CustomerNode* temp = current->next;
            current->next = temp->next;
            delete temp;
            saveToFile();
            return true;
        }
        current = current->next;
    }
    return false;
}

// ==================== JSON OUTPUT ====================
string CustomerList::getCustomerJSON(CustomerNode* customer) {
    if (customer == nullptr) return "{}";

    string json = "{";
    json += "\"id\":" + to_string(customer->id) + ",";
    json += "\"name\":\"" + escapeJSON(customer->name) + "\",";
    json += "\"email\":\"" + escapeJSON(customer->email) + "\",";
    json += "\"phone\":\"" + escapeJSON(customer->phone) + "\",";
    json += "\"address\":\"" + escapeJSON(customer->address) + "\"";
    json += "}";

    return json;
}

string CustomerList::getAllCustomersJSON() {
    string json = "[";
    bool first = true;
    CustomerNode* current = head;
    
    while (current != nullptr) {
        if (!first) json += ",";
        json += "{";
        json += "\"id\":" + to_string(current->id) + ",";
        json += "\"name\":\"" + current->name + "\",";
        json += "\"email\":\"" + current->email + "\",";
        json += "\"phone\":\"" + current->phone + "\",";
        json += "\"address\":\"" + current->address + "\"";
        json += "}";
        first = false;
        current = current->next;
    }
    
    json += "]";
    return json;
}
