#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

// ==================== CUSTOMER NODE ====================
struct CustomerNode {
    int id;
    string name;
    string email;
    string password;
    string phone;
    string address;
    CustomerNode* next;

    CustomerNode(int cid, string cname, string cemail = "", string cpassword = "", 
                 string cphone = "", string caddress = "") {
        id = cid;
        name = cname;
        email = cemail;
        password = cpassword;
        phone = cphone;
        address = caddress;
        next = nullptr;
    }
};

// ==================== CUSTOMER LIST ====================
class CustomerList {
private:
    CustomerNode* head;
    int nextId;
    string filename;

public:
    CustomerList();
    CustomerList(string file);
    ~CustomerList();

    void addCustomer(int id, string name) {
        CustomerNode* newCustomer = new CustomerNode(id, name);
        if (!head) head = newCustomer;
        else {
            CustomerNode* temp = head;
            while (temp->next) temp = temp->next;
            temp->next = newCustomer;
        }
    }

    CustomerNode* getCustomer(int id) {
        CustomerNode* temp = head;
        while (temp) {
            if (temp->id == id) return temp;
            temp = temp->next;
        }
        return nullptr;
    }

    int registerCustomer(string name, string email, string password, string phone, string address);
    CustomerNode* login(string email, string password);
    CustomerNode* findById(int id);
    bool updateCustomer(int id, string name, string phone, string address);
    string getCustomerJSON(CustomerNode* customer);

    void saveToFile();
    void loadFromFile();
};

#endif
