#define _WIN32_WINNT 0x0A00 // Enable Windows 10 APIs

#include "httplib.h"  // HTTP server library

// Header files
#include "event.h"
#include "customer.h"
#include "staff.h"   
#include "supervisor.h" 
#include "eventstack.h" 
#include "utils.h"    

// Standard libraries
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>    // String streams
#include <map>        // Map data structure

using namespace std;
using namespace httplib;

// ==================== GLOBAL DATA STRUCTURES ====================
EventList eventList("events.txt");
StaffList staffList("staff.txt");
SupervisorList supervisorList("supervisor.txt");
TaskAssignmentList taskList("tasks.txt");
EventStack eventHistory;
CustomerList customerList("customers.txt");

map<string, int> activeSessions;
map<string, string> sessionRoles;
int sessionCounter = 1000;

// ==================== VALIDATION HELPERS ====================
bool isValidPhone(string phone) {
    if (phone.length() != 11) return false;
    if (phone[0] != '0' || phone[1] != '3') return false;
    for (char c : phone) {
        if (!isdigit(c)) return false;
    }
    return true;
}

bool isValidEmail(string email) {
    size_t atPos = email.find('@');
    size_t dotPos = email.find('.');
    if (atPos == string::npos || dotPos == string::npos) return false;
    if (atPos == 0 || dotPos == 0) return false;
    if (atPos > dotPos) return false;
    return true;
}

bool isValidRole(string role) {
    return (role == "Event Planner" || role == "Event Manager");
}

// ==================== JSON PARSING ====================
string getJsonValue(const string& json, const string& key) {
    string searchKey = "\"" + key + "\"";
    size_t keyPos = json.find(searchKey);
    if (keyPos == string::npos) return "";
    size_t colonPos = json.find(":", keyPos);
    if (colonPos == string::npos) return "";
    size_t startPos = colonPos + 1;
    while (startPos < json.length() && (json[startPos] == ' ' || json[startPos] == '\t')) {
        startPos++;
    }
    if (startPos < json.length() && json[startPos] == '"') {
        startPos++;
        size_t endPos = startPos;
        while (endPos < json.length() && json[endPos] != '"') {
            endPos++;
        }
        if (endPos < json.length()) {
            return json.substr(startPos, endPos - startPos);
        }
    }
    return "";
}

int getJsonInt(const string& json, const string& key) {
    size_t pos = json.find("\"" + key + "\"");
    if (pos == string::npos) return 0;
    pos = json.find(":", pos);
    if (pos == string::npos) return 0;
    pos = json.find_first_of("0123456789", pos);
    if (pos == string::npos) return 0;
    size_t endPos = json.find_first_not_of("0123456789", pos);
    string numStr = json.substr(pos, endPos - pos);
    return stoi(numStr);
}

// ==================== SESSION HELPERS ====================
string generateSessionId() {
    return "SESSION_" + to_string(sessionCounter++);
}

int getUserIdFromRequest(const Request& req, string& role) {
    string sessionId = req.get_header_value("X-Session-ID");
    if (sessionId.empty()) {
        if (req.has_header("Cookie")) {
            string cookie = req.get_header_value("Cookie");
            size_t pos = cookie.find("sessionId=");
            if (pos != string::npos) {
                size_t end = cookie.find(";", pos);
                if (end == string::npos) end = cookie.length();
                sessionId = cookie.substr(pos + 10, end - (pos + 10));
            }
        }
    }
    if (!sessionId.empty() && activeSessions.count(sessionId)) {
        role = sessionRoles[sessionId];
        return activeSessions[sessionId];
    }
    return -1;
}

// ==================== SERVICE COSTS ====================
map<string, int> getServiceCosts() {
    map<string, int> costs;
    costs["catering"] = 1000;
    costs["decoration"] = 10000;
    costs["transport"] = 1000;
    costs["photography"] = 15000;
    costs["music"] = 20000;
    costs["planning"] = 30000;
    return costs;
}

int calculateServiceCost(const string& servicesStr, int guests) {
    map<string, int> costs = getServiceCosts();
    int total = 0;
    if (servicesStr.empty()) return 0;
    stringstream ss(servicesStr);
    string service;
    while (getline(ss, service, ',')) {
        service.erase(0, service.find_first_not_of(" \t"));
        service.erase(service.find_last_not_of(" \t") + 1);
        if (costs.find(service) != costs.end()) {
            if (service == "catering" || service == "transport") {
                total += costs[service] * guests;
            } else {
                total += costs[service];
            }
        }
    }
    return total;
}

// ==================== MAIN SERVER ====================

int main() {
    Server svr;

    // Set default headers for all responses to allow cross-origin requests
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},  // Allow requests from any origin
        {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},  // Allow these HTTP methods
        {"Access-Control-Allow-Headers", "Content-Type, X-Session-ID"}  // Allow these header types
    });

    // Handle OPTIONS requests (used by browsers before POST/PUT/DELETE)
    svr.Options("/(.*)", [](const Request& req, Response& res) {
        res.status = 204;  // Return 204 No Content for OPTIONS requests
    });

    // ==================== SERVE HTML FILES ====================
    // These routes serve the HTML files when users visit different URLs
    
    // Route: GET / (root URL - serves index.html)
    svr.Get("/", [](const Request&, Response& res) {
        // Open index.html file
        ifstream file("index.html");
        if (file.is_open()) {
            // Read entire file into buffer
            stringstream buffer;
            buffer << file.rdbuf();
            // Send file content as HTML response
            res.set_content(buffer.str(), "text/html");
        } else {
            // If file not found, return 404 error
            res.status = 404;
        }
    });

    // Route: GET /login (serves login.html)
    svr.Get("/login", [](const Request&, Response& res) {
        ifstream file("login.html");
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
        }
    });

    // Route: GET /about (serves about.html)
    svr.Get("/about", [](const Request&, Response& res) {
        ifstream file("about.html");
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
        }
    });

    // Route: GET /contact (serves contact.html)
    svr.Get("/contact", [](const Request&, Response& res) {
        ifstream file("contact.html");
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
        }
    });

    // Route: GET /register (serves customer.html for registration)
    svr.Get("/register", [](const Request&, Response& res) {
        ifstream file("customer.html");
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
        }
    });

    // Route: GET /staff (serves staff.html)
    svr.Get("/staff", [](const Request&, Response& res) {
        ifstream file("staff.html");
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
        }
    });

    // Route: GET /admin (serves admin.html)
    svr.Get("/admin", [](const Request&, Response& res) {
        ifstream file("admin.html");
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
        }
    });

    // Route: GET /supervisor (serves supervisor.html)
    svr.Get("/supervisor", [](const Request&, Response& res) {
        ifstream file("supervisor.html");
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
        }
    });

    // Catch-all route: Serve any .html file directly by filename
    // For example: GET /mypage.html will serve mypage.html
    svr.Get("/(.*\\.html)", [](const Request& req, Response& res) {
        // Extract filename from URL (e.g., "mypage.html")
        string filename = req.matches[1];
        ifstream file(filename);
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
            file.close();
        } else {
            // If file not found
            res.status = 404;
            res.set_content("File not found", "text/html");
        }
    });

    // ==================== STATIC FILES (IMAGES) ====================
    // This route serves image files (PNG, JPG, GIF, etc.)
    
    // Route: GET /images/* (serves files from images folder)
    // Example: GET /images/logo.png will serve logo.png from images folder
    svr.Get("/images/(.*)", [](const Request& req, Response& res) {
        // Extract filename from URL
        string filename = req.matches[1];
        // Build full path to image file
        string filepath = "images/" + filename;
        
        // Open image file in binary mode
        ifstream file(filepath, ios::binary);
        if (file.is_open()) {
            // Read entire file into buffer
            stringstream buffer;
            buffer << file.rdbuf();
            
            // Determine content type based on file extension
            string contentType = "image/png";  // Default to PNG
            if (filename.find(".jpg") != string::npos || filename.find(".jpeg") != string::npos) {
                contentType = "image/jpeg";  // JPG/JPEG files
            } else if (filename.find(".png") != string::npos) {
                contentType = "image/png";  // PNG files
            } else if (filename.find(".gif") != string::npos) {
                contentType = "image/gif";  // GIF files
            }
            
            // Send file content with correct image type
            res.set_content(buffer.str(), contentType);
            file.close();
        } else {
            // If image not found
            res.status = 404;
            res.set_content("Image not found", "text/plain");
        }
    });

    // ==================== AUTHENTICATION ENDPOINTS ====================
    
    // Customer Registration
    svr.Post("/api/auth/register", [](const Request& req, Response& res) {
        string body = req.body;
        
        string name = getJsonValue(body, "name");
        string email = getJsonValue(body, "email");
        string password = getJsonValue(body, "password");
        string phone = getJsonValue(body, "phone");
        string address = getJsonValue(body, "address");
        
        if (name.empty() || email.empty() || password.empty() || phone.empty() || address.empty()) {
            res.set_content("{\"success\":false,\"message\":\"Missing required fields\"}", "application/json");
            return;
        }
        
        if (!isValidPhone(phone)) {
            res.set_content("{\"success\":false,\"message\":\"Phone must be 11 digits starting with 03\"}", "application/json");
            return;
        }
        
        if (!isValidEmail(email)) {
            res.set_content("{\"success\":false,\"message\":\"Invalid email format\"}", "application/json");
            return;
        }
        
        int customerId = customerList.registerCustomer(name, email, password, phone, address);
        
        if (customerId == -1) {
            res.set_content("{\"success\":false,\"message\":\"Email already exists\"}", "application/json");
        } else {
            string sessionId = generateSessionId();
            activeSessions[sessionId] = customerId;
            sessionRoles[sessionId] = "customer";
            
            CustomerNode* customer = customerList.findById(customerId);
            string response = "{\"success\":true,\"message\":\"Registration successful\",";
            response += "\"sessionId\":\"" + sessionId + "\",";
            response += "\"customer\":" + customerList.getCustomerJSON(customer) + "}";
            
            res.set_header("Set-Cookie", "sessionId=" + sessionId + "; Path=/; HttpOnly");
            res.set_content(response, "application/json");
        }
    });
    
    // Customer Login
    svr.Post("/api/auth/login", [](const Request& req, Response& res) {
        string body = req.body;
        
        string email = getJsonValue(body, "email");
        string password = getJsonValue(body, "password");
        
        CustomerNode* customer = customerList.login(email, password);
        
        if (customer == nullptr) {
            res.set_content("{\"success\":false,\"message\":\"Invalid email or password\"}", "application/json");
        } else {
            string sessionId = generateSessionId();
            activeSessions[sessionId] = customer->id;
            sessionRoles[sessionId] = "customer";
            
            string response = "{\"success\":true,\"message\":\"Login successful\",";
            response += "\"sessionId\":\"" + sessionId + "\",";
            response += "\"customer\":" + customerList.getCustomerJSON(customer) + "}";
            
            res.set_header("Set-Cookie", "sessionId=" + sessionId + "; Path=/; HttpOnly");
            res.set_content(response, "application/json");
        }
    });
    
    // Admin Login
    svr.Post("/api/auth/admin-login", [](const Request& req, Response& res) {
        string body = req.body;
        string email = getJsonValue(body, "email");
        string password = getJsonValue(body, "password");
        
        email.erase(0, email.find_first_not_of(" \t\r\n"));
        email.erase(email.find_last_not_of(" \t\r\n") + 1);
        password.erase(0, password.find_first_not_of(" \t\r\n"));
        password.erase(password.find_last_not_of(" \t\r\n") + 1);
        
        if (email == "admin@afsanayevents.com" && password == "admin123") {
            string sessionId = generateSessionId();
            activeSessions[sessionId] = 9999;
            sessionRoles[sessionId] = "admin";
            
            string response = "{\"success\":true,\"message\":\"Admin login successful\",";
            response += "\"sessionId\":\"" + sessionId + "\",";
            response += "\"admin\":{\"id\":9999,\"name\":\"Administrator\",\"email\":\"admin@afsanayevents.com\"}}";
            
            res.set_header("Set-Cookie", "sessionId=" + sessionId + "; Path=/; HttpOnly");
            res.set_content(response, "application/json");
        } else {
            res.set_content("{\"success\":false,\"message\":\"Invalid admin credentials\"}", "application/json");
        }
    });
    
    // Staff Login
    svr.Post("/api/auth/staff-login", [](const Request& req, Response& res) {
        string body = req.body;
        string email = getJsonValue(body, "email");
        string password = getJsonValue(body, "password");
        
        StaffNode* staff = staffList.loginStaff(email, password);
        
        if (staff == nullptr) {
            res.set_content("{\"success\":false,\"message\":\"Invalid email or password\"}", "application/json");
        } else {
            string sessionId = generateSessionId();
            activeSessions[sessionId] = staff->id;
            sessionRoles[sessionId] = "staff";
            
            string response = "{\"success\":true,\"message\":\"Staff login successful\",";
            response += "\"sessionId\":\"" + sessionId + "\",";
            response += "\"staff\":" + staffList.getStaffJSON(staff) + "}";
            
            res.set_header("Set-Cookie", "sessionId=" + sessionId + "; Path=/; HttpOnly");
            res.set_content(response, "application/json");
        }
    });

    // Supervisor Login
    svr.Post("/api/auth/supervisor-login", [](const Request& req, Response& res) {
        string body = req.body;
        string email = getJsonValue(body, "email");
        string password = getJsonValue(body, "password");

        email.erase(0, email.find_first_not_of(" \t\r\n"));
        email.erase(email.find_last_not_of(" \t\r\n") + 1);
        password.erase(0, password.find_first_not_of(" \t\r\n"));
        password.erase(password.find_last_not_of(" \t\r\n") + 1);

        SupervisorNode* supervisor = supervisorList.login(email, password);

        if (supervisor != nullptr) {
            string sessionId = generateSessionId();
            activeSessions[sessionId] = supervisor->id;
            sessionRoles[sessionId] = "supervisor";

            string response = "{\"success\":true,\"message\":\"Supervisor login successful\",";
            response += "\"sessionId\":\"" + sessionId + "\",";
            response += "\"supervisor\":" + supervisorList.getSupervisorJSON(supervisor) + "}";
            
            res.set_header("Set-Cookie", "sessionId=" + sessionId + "; Path=/; HttpOnly");
            res.set_content(response, "application/json");
        } else {
            res.set_content("{\"success\":false,\"message\":\"Invalid email or password\"}", "application/json");
        }
    });
    
    // Logout
    svr.Post("/api/auth/logout", [](const Request& req, Response& res) {
        string sessionId = req.get_header_value("X-Session-ID");
        if (sessionId.empty() && req.has_header("Cookie")) {
            string cookie = req.get_header_value("Cookie");
            size_t pos = cookie.find("sessionId=");
            if (pos != string::npos) {
                sessionId = cookie.substr(pos + 10, 24);
            }
        }
        
        if (!sessionId.empty()) {
            activeSessions.erase(sessionId);
            sessionRoles.erase(sessionId);
        }
        
        res.set_header("Set-Cookie", "sessionId=; Path=/; HttpOnly; Max-Age=0");
        res.set_content("{\"success\":true,\"message\":\"Logout successful\"}", "application/json");
    });

    // ==================== CUSTOMER PROFILE ====================
    
    svr.Get("/api/customer/profile", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId == -1 || role != "customer") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        CustomerNode* customer = customerList.findById(userId);
        string response = "{\"success\":true,\"customer\":" + customerList.getCustomerJSON(customer) + "}";
        res.set_content(response, "application/json");
    });
    
    svr.Put("/api/customer/profile", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId == -1 || role != "customer") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        string body = req.body;
        string name = getJsonValue(body, "name");
        string phone = getJsonValue(body, "phone");
        string address = getJsonValue(body, "address");
        
        bool success = customerList.updateCustomer(userId, name, phone, address);
        
        if (success) {
            res.set_content("{\"success\":true,\"message\":\"Profile updated successfully\"}", "application/json");
        } else {
            res.set_content("{\"success\":false,\"message\":\"Failed to update profile\"}", "application/json");
        }
    });

    // ==================== EVENTS ====================
    
    svr.Get("/api/events", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId != -1 && role == "customer") {
            string json = eventList.getCustomerEventsWithSupervisorJSON(userId, &supervisorList);
            res.set_content(json, "application/json");
        } else {
            res.set_content("[]", "application/json");
        }
    });

    svr.Post("/api/events", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId == -1 || role != "customer") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        string body = req.body;
        
        string name = getJsonValue(body, "name");
        string type = getJsonValue(body, "type");
        string date = getJsonValue(body, "date");
        string time = getJsonValue(body, "time");
        string location = getJsonValue(body, "location");
        string address = getJsonValue(body, "address");
        int guests = getJsonInt(body, "guests");
        string description = getJsonValue(body, "description");
        string services = getJsonValue(body, "services");
        
        int id = eventList.addEvent(userId, name, type, date, time, location, address, guests, description, services);
        
        string response = "{\"success\":true,\"message\":\"Event added successfully\",\"id\":" + to_string(id) + "}";
        res.set_content(response, "application/json");
    });

    svr.Put("/api/events/(\\d+)", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId == -1 || role != "customer") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        int id = stoi(req.matches[1]);
        
        EventNode* event = eventList.getEvent(id);
        if (event == nullptr || event->customerId != userId) {
            res.set_content("{\"success\":false,\"message\":\"Event not found or unauthorized\"}", "application/json");
            return;
        }

        if (event->status == "accepted") {
            res.set_content("{\"success\":false,\"message\":\"Event already accepted; cannot edit\"}", "application/json");
            return;
        }
        
        string body = req.body;
        
        string name = getJsonValue(body, "name");
        string type = getJsonValue(body, "type");
        string date = getJsonValue(body, "date");
        string time = getJsonValue(body, "time");
        string location = getJsonValue(body, "location");
        string address = getJsonValue(body, "address");
        int guests = getJsonInt(body, "guests");
        string description = getJsonValue(body, "description");
        string services = getJsonValue(body, "services");
        
        bool success = eventList.updateEvent(id, name, type, date, time, location, address, guests, description, services);
        
        if (success) {
            res.set_content("{\"success\":true,\"message\":\"Event updated successfully\"}", "application/json");
        } else {
            res.set_content("{\"success\":false,\"message\":\"Event not found\"}", "application/json");
        }
    });

    svr.Delete("/api/events/(\\d+)", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId == -1 || role != "customer") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        int id = stoi(req.matches[1]);
        
        EventNode* event = eventList.getEvent(id);
        if (event == nullptr || event->customerId != userId) {
            res.set_content("{\"success\":false,\"message\":\"Event not found or unauthorized\"}", "application/json");
            return;
        }

        if (event->status == "accepted" || event->status == "confirmed") {
            res.set_content("{\"success\":false,\"message\":\"Event already confirmed; cannot delete\"}", "application/json");
            return;
        }
        
        EventNode* historyCopy = new EventNode(
            event->id, event->customerId, event->name, event->type, event->date, event->time,
            event->location, event->address, event->guests, event->description, event->services, "completed"
        );
        eventHistory.push(historyCopy);
        
        bool success = eventList.deleteEvent(id);
        
        if (success) {
            res.set_content("{\"success\":true,\"message\":\"Event deleted successfully\"}", "application/json");
        } else {
            res.set_content("{\"success\":false,\"message\":\"Event not found\"}", "application/json");
        }
    });

    svr.Get("/api/history", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId == -1 || role != "customer") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        string json = eventHistory.getCustomerHistoryJSON(userId);
        res.set_content(json, "application/json");
    });

    svr.Post("/api/events/(\\d+)/accept", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId == -1 || (role != "admin" && role != "staff")) {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized\"}", "application/json");
            return;
        }

        int id = stoi(req.matches[1]);
        EventNode* event = eventList.getEvent(id);
        if (!event) {
            res.set_content("{\"success\":false,\"message\":\"Event not found\"}", "application/json");
            return;
        }

        event->status = "accepted";
        eventList.saveToFile();

        res.set_content("{\"success\":true,\"message\":\"Event accepted\"}", "application/json");
    });

    // ==================== STAFF MANAGEMENT ====================
    
    svr.Get("/api/staff", [](const Request& req, Response& res) {
        string json = staffList.getAllStaffJSON();
        res.set_content(json, "application/json");
    });
    
    svr.Get("/api/supervisors", [](const Request& req, Response& res) {
        string json = supervisorList.getAllSupervisorsJSON();
        res.set_content(json, "application/json");
    });
    
    svr.Post("/api/admin/staff/add", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId != 9999 || role != "admin") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized - Admin access required\"}", "application/json");
            return;
        }
        
        string body = req.body;
        
        string name = getJsonValue(body, "name");
        string email = getJsonValue(body, "email");
        string password = getJsonValue(body, "password");
        string phone = getJsonValue(body, "phone");
        string staffRole = getJsonValue(body, "role");
        
        if (name.empty() || email.empty() || password.empty() || staffRole.empty() || phone.empty()) {
            res.set_content("{\"success\":false,\"message\":\"Missing required fields\"}", "application/json");
            return;
        }
        
        if (!isValidPhone(phone)) {
            res.set_content("{\"success\":false,\"message\":\"Phone must be 11 digits starting with 03\"}", "application/json");
            return;
        }
        
        if (!isValidEmail(email)) {
            res.set_content("{\"success\":false,\"message\":\"Invalid email format\"}", "application/json");
            return;
        }
        
        int newId = staffList.getNextId();
        
        staffList.addStaff(newId, name, staffRole, email, password, phone, "available");
        staffList.saveToFile();
        
        string response = "{\"success\":true,\"message\":\"Staff added successfully\",";
        response += "\"staff\":{\"id\":" + to_string(newId) + ",\"name\":\"" + name + "\",";
        response += "\"role\":\"" + staffRole + "\",\"email\":\"" + email + "\",\"phone\":\"" + phone + "\"}}";
        res.set_content(response, "application/json");
    });

    svr.Put("/api/admin/staff/(\\d+)", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId != 9999 || role != "admin") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized - Admin access required\"}", "application/json");
            return;
        }
        
        int staffId = stoi(req.matches[1]);
        StaffNode* staff = staffList.getStaff(staffId);
        
        if (staff == nullptr) {
            res.set_content("{\"success\":false,\"message\":\"Staff not found\"}", "application/json");
            return;
        }
        
        string body = req.body;
        string name = getJsonValue(body, "name");
        string email = getJsonValue(body, "email");
        string phone = getJsonValue(body, "phone");
        string staffRole = getJsonValue(body, "role");
        
        if (name.empty() || email.empty() || phone.empty() || staffRole.empty()) {
            res.set_content("{\"success\":false,\"message\":\"All fields are required\"}", "application/json");
            return;
        }
        
        if (!isValidPhone(phone)) {
            res.set_content("{\"success\":false,\"message\":\"Phone must be 11 digits starting with 03\"}", "application/json");
            return;
        }
        
        staff->name = name;
        staff->email = email;
        staff->phone = phone;
        staff->role = staffRole;
        
        staffList.saveToFile();
        
        string response = "{\"success\":true,\"message\":\"Staff updated successfully\",";
        response += "\"staff\":" + staffList.getStaffJSON(staff) + "}";
        res.set_content(response, "application/json");
    });

    svr.Delete("/api/admin/staff/(\\d+)", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId != 9999 || role != "admin") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized - Admin access required\"}", "application/json");
            return;
        }
        
        int staffId = stoi(req.matches[1]);
        // ... continuing from Part 1 ...
    
        bool success = staffList.deleteStaff(staffId);
        
        if (success) {
            staffList.saveToFile();
            res.set_content("{\"success\":true,\"message\":\"Staff deleted successfully\"}", "application/json");
        } else {
            res.set_content("{\"success\":false,\"message\":\"Staff not found\"}", "application/json");
        }
    });

    svr.Post("/api/admin/supervisor/add", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);

        if (userId != 9999 || role != "admin") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized - Admin access required\"}", "application/json");
            return;
        }

        string body = req.body;

        string name = getJsonValue(body, "name");
        string email = getJsonValue(body, "email");
        string password = getJsonValue(body, "password");
        string phone = getJsonValue(body, "phone");
        string supRole = getJsonValue(body, "role");
        string address = getJsonValue(body, "address");

        if (name.empty() || email.empty() || password.empty() || supRole.empty() || phone.empty() || address.empty()) {
            res.set_content("{\"success\":false,\"message\":\"Missing required fields\"}", "application/json");
            return;
        }

        if (!isValidPhone(phone)) {
            res.set_content("{\"success\":false,\"message\":\"Phone must be 11 digits starting with 03\"}", "application/json");
            return;
        }

        if (!isValidEmail(email)) {
            res.set_content("{\"success\":false,\"message\":\"Invalid email format\"}", "application/json");
            return;
        }

        if (!isValidRole(supRole)) {
            res.set_content("{\"success\":false,\"message\":\"Role must be 'Event Planner' or 'Event Manager'\"}", "application/json");
            return;
        }

        int newId = supervisorList.getNextId();

        supervisorList.addSupervisor(newId, name, email, password, phone, supRole, address);
        supervisorList.saveToFile();

        string response = "{\"success\":true,\"message\":\"Supervisor added successfully\",";
        response += "\"supervisor\":{\"id\":" + to_string(newId) + ",\"name\":\"" + name + "\",";
        response += "\"email\":\"" + email + "\",\"phone\":\"" + phone + 
                   "\",\"role\":\"" + supRole + "\",\"address\":\"" + address + "\"}}";
        res.set_content(response, "application/json");
    });

    // ==================== SUPERVISOR TASK MANAGEMENT ====================
    
    svr.Post("/api/supervisor/task/create", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId == -1 || role != "supervisor") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized - Supervisor access required\"}", "application/json");
            return;
        }
        
        string body = req.body;
        int staffId = getJsonInt(body, "staffId");
        string description = getJsonValue(body, "description");
        string taskDetails = getJsonValue(body, "taskDetails");
        string location = getJsonValue(body, "location");
        string dueDate = getJsonValue(body, "dueDate");
        
        if (staffId == 0 || description.empty() || location.empty() || dueDate.empty()) {
            res.set_content("{\"success\":false,\"message\":\"Missing required fields\"}", "application/json");
            return;
        }
        
        int taskId = taskList.createTask(userId, staffId, description, taskDetails, location, dueDate);
        
        string response = "{\"success\":true,\"message\":\"Task created and assigned\",";
        response += "\"task\":{\"id\":" + to_string(taskId) + ",\"supervisorId\":" + to_string(userId) + ",";
        response += "\"staffId\":" + to_string(staffId) + ",\"description\":\"" + description + "\",";
        response += "\"taskDetails\":\"" + taskDetails + "\",\"location\":\"" + location + "\",\"status\":\"pending\",\"dueDate\":\"" + dueDate + "\"}}";
        res.set_content(response, "application/json");
    });
    
    svr.Get("/api/supervisor/tasks", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId == -1 || role != "supervisor") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        string json = taskList.getAllTasksJSON();
        res.set_content(json, "application/json");
    });

    svr.Get("/api/tasks", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId == -1 || role != "staff") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        string json = taskList.getStaffTasksJSON(userId);
        res.set_content(json, "application/json");
    });

    svr.Put("/api/supervisor/task/(\\d+)/status", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId == -1) {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        int taskId = stoi(req.matches[1]);
        string body = req.body;
        string status = getJsonValue(body, "status");
        
        if (status.empty()) {
            res.set_content("{\"success\":false,\"message\":\"Missing status field\"}", "application/json");
            return;
        }
        
        bool success = taskList.updateTaskStatus(taskId, status);
        
        if (success) {
            res.set_content("{\"success\":true,\"message\":\"Task status updated\"}", "application/json");
        } else {
            res.set_content("{\"success\":false,\"message\":\"Task not found\"}", "application/json");
        }
    });
    
    svr.Delete("/api/supervisor/task/(\\d+)", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId == -1 || role != "supervisor") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        int taskId = stoi(req.matches[1]);
        bool success = taskList.deleteTask(taskId);
        
        if (success) {
            res.set_content("{\"success\":true,\"message\":\"Task deleted\"}", "application/json");
        } else {
            res.set_content("{\"success\":false,\"message\":\"Task not found\"}", "application/json");
        }
    });
    
    svr.Put("/api/supervisor/staff/(\\d+)", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId == -1 || role != "supervisor") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        int staffId = stoi(req.matches[1]);
        string body = req.body;
        
        string name = getJsonValue(body, "name");
        string email = getJsonValue(body, "email");
        string phone = getJsonValue(body, "phone");
        string staffRole = getJsonValue(body, "role");
        
        StaffNode* staff = staffList.getStaff(staffId);
        if (!staff) {
            res.set_content("{\"success\":false,\"message\":\"Staff not found\"}", "application/json");
            return;
        }
        
        if (!name.empty()) staff->name = name;
        if (!email.empty()) staff->email = email;
        if (!phone.empty()) {
            if (!isValidPhone(phone)) {
                res.set_content("{\"success\":false,\"message\":\"Phone must be 11 digits starting with 03\"}", "application/json");
                return;
            }
            staff->phone = phone;
        }
        if (!staffRole.empty()) staff->role = staffRole;
        
        staffList.saveToFile();
        
        string response = "{\"success\":true,\"message\":\"Staff updated\",\"staff\":" + 
                         staffList.getStaffJSON(staff) + "}";
        res.set_content(response, "application/json");
    });
    
    svr.Get("/api/supervisor/event-requests", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId == -1 || role != "supervisor") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        string json = "[";
        bool first = true;
        EventNode* current = eventList.getHead();
        
        while (current != nullptr) {
            if (current->status == "pending" && current->assignedSupervisorId == 0) {
                if (!first) json += ",";
                json += "{\"id\":" + to_string(current->id) + ",";
                json += "\"customerId\":" + to_string(current->customerId) + ",";
                json += "\"name\":\"" + current->name + "\",";
                json += "\"type\":\"" + current->type + "\",";
                json += "\"date\":\"" + current->date + "\",";
                json += "\"guests\":" + to_string(current->guests) + ",";
                json += "\"status\":\"" + current->status + "\"}";
                first = false;
            }
            current = current->next;
        }
        json += "]";
        res.set_content(json, "application/json");
    });
    
    svr.Post("/api/supervisor/accept-event/(\\d+)", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId == -1 || role != "supervisor") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        int eventId = stoi(req.matches[1]);
        EventNode* event = eventList.getEvent(eventId);
        
        if (event == nullptr) {
            res.set_content("{\"success\":false,\"message\":\"Event not found\"}", "application/json");
            return;
        }
        
        SupervisorNode* supervisor = supervisorList.findById(userId);
        if (!supervisor) {
            res.set_content("{\"success\":false,\"message\":\"Supervisor not found\"}", "application/json");
            return;
        }
        
        event->assignedSupervisorId = userId;
        event->status = "confirmed";
        eventList.saveToFile();
        supervisorList.incrementAssignedEvents(userId);
        
        string response = "{\"success\":true,\"message\":\"Event accepted\",";
        response += "\"event\":{\"id\":" + to_string(event->id) + ",\"status\":\"confirmed\",";
        response += "\"assignedSupervisor\":{\"id\":" + to_string(supervisor->id) + ",";
        response += "\"name\":\"" + supervisor->name + "\",\"phone\":\"" + supervisor->phone + "\",";
        response += "\"email\":\"" + supervisor->email + "\"}}}";
        res.set_content(response, "application/json");
    });

    svr.Get("/api/supervisor/events", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId == -1 || role != "supervisor") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        string json = eventList.getSupervisorEventsJSON(userId);
        res.set_content(json, "application/json");
    });

    svr.Get("/api/supervisor/profile", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId == -1 || role != "supervisor") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        SupervisorNode* supervisor = supervisorList.findById(userId);
        if (supervisor == nullptr) {
            res.set_content("{\"success\":false,\"message\":\"Supervisor not found\"}", "application/json");
            return;
        }
        
        string response = "{\"success\":true,\"supervisor\":" + supervisorList.getSupervisorJSON(supervisor) + "}";
        res.set_content(response, "application/json");
    });

    // ==================== ADMIN CUSTOMER MANAGEMENT ====================
    
    svr.Get("/api/admin/customers", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId != 9999 || role != "admin") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized - Admin access required\"}", "application/json");
            return;
        }
        
        string json = customerList.getAllCustomersJSON();
        res.set_content(json, "application/json");
    });
    
    svr.Delete("/api/admin/customers/(\\d+)", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId != 9999 || role != "admin") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized - Admin access required\"}", "application/json");
            return;
        }
        
        int customerId = stoi(req.matches[1]);
        
        bool success = customerList.deleteCustomer(customerId);
        customerList.saveToFile();
        
        if (success) {
            res.set_content("{\"success\":true,\"message\":\"Customer deleted successfully\"}", "application/json");
        } else {
            res.set_content("{\"success\":false,\"message\":\"Customer not found\"}", "application/json");
        }
    });

    svr.Put("/api/admin/customers/(\\d+)", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId != 9999 || role != "admin") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized - Admin access required\"}", "application/json");
            return;
        }
        
        int customerId = stoi(req.matches[1]);
        string body = req.body;
        
        string name = getJsonValue(body, "name");
        string email = getJsonValue(body, "email");
        string phone = getJsonValue(body, "phone");
        string address = getJsonValue(body, "address");
        
        if (name.empty() || email.empty() || phone.empty() || address.empty()) {
            res.set_content("{\"success\":false,\"message\":\"Missing required fields\"}", "application/json");
            return;
        }
        
        if (!isValidPhone(phone)) {
            res.set_content("{\"success\":false,\"message\":\"Phone must be 11 digits starting with 03\"}", "application/json");
            return;
        }
        
        if (!isValidEmail(email)) {
            res.set_content("{\"success\":false,\"message\":\"Invalid email format\"}", "application/json");
            return;
        }
        
        CustomerNode* customer = customerList.findById(customerId);
        if (customer == nullptr) {
            res.set_content("{\"success\":false,\"message\":\"Customer not found\"}", "application/json");
            return;
        }
        
        customer->name = name;
        customer->email = email;
        customer->phone = phone;
        customer->address = address;
        customerList.saveToFile();
        
        string response = "{\"success\":true,\"message\":\"Customer updated successfully\",";
        response += "\"customer\":" + customerList.getCustomerJSON(customer) + "}";
        res.set_content(response, "application/json");
    });

    // ==================== ADMIN SUPERVISOR MANAGEMENT ====================
    
    svr.Get("/api/admin/supervisors", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId != 9999 || role != "admin") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized - Admin access required\"}", "application/json");
            return;
        }
        
        string json = supervisorList.getAllSupervisorsJSON();
        res.set_content(json, "application/json");
    });
    
    svr.Delete("/api/admin/supervisors/(\\d+)", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId != 9999 || role != "admin") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized - Admin access required\"}", "application/json");
            return;
        }
        
        int supervisorId = stoi(req.matches[1]);
        
        bool success = supervisorList.deleteSupervisor(supervisorId);
        supervisorList.saveToFile();
        
        if (success) {
            res.set_content("{\"success\":true,\"message\":\"Supervisor deleted successfully\"}", "application/json");
        } else {
            res.set_content("{\"success\":false,\"message\":\"Supervisor not found\"}", "application/json");
        }
    });

    svr.Put("/api/admin/supervisors/(\\d+)", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId != 9999 || role != "admin") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized - Admin access required\"}", "application/json");
            return;
        }
        
        int supervisorId = stoi(req.matches[1]);
        string body = req.body;
        
        string name = getJsonValue(body, "name");
        string email = getJsonValue(body, "email");
        string phone = getJsonValue(body, "phone");
        string supRole = getJsonValue(body, "role");
        
        if (name.empty() || email.empty() || phone.empty() || supRole.empty()) {
            res.set_content("{\"success\":false,\"message\":\"Missing required fields\"}", "application/json");
            return;
        }
        
        if (!isValidPhone(phone)) {
            res.set_content("{\"success\":false,\"message\":\"Phone must be 11 digits starting with 03\"}", "application/json");
            return;
        }
        
        if (!isValidEmail(email)) {
            res.set_content("{\"success\":false,\"message\":\"Invalid email format\"}", "application/json");
            return;
        }
        
        if (!isValidRole(supRole)) {
            res.set_content("{\"success\":false,\"message\":\"Role must be 'Event Planner' or 'Event Manager'\"}", "application/json");
            return;
        }
        
        bool success = supervisorList.updateSupervisor(supervisorId, name, email, phone, supRole);
        
        if (success) {
            supervisorList.saveToFile();
            SupervisorNode* supervisor = supervisorList.findById(supervisorId);
            string response = "{\"success\":true,\"message\":\"Supervisor updated successfully\",";
            response += "\"supervisor\":" + supervisorList.getSupervisorJSON(supervisor) + "}";
            res.set_content(response, "application/json");
        } else {
            res.set_content("{\"success\":false,\"message\":\"Supervisor not found\"}", "application/json");
        }
    });

    // ==================== ADMIN EVENT MANAGEMENT ====================
    
    svr.Get("/api/admin/events", [](const Request& req, Response& res) {
        string role;
        int userId = getUserIdFromRequest(req, role);
        
        if (userId != 9999 || role != "admin") {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized - Admin access required\"}", "application/json");
            return;
        }
        
        string json = eventList.getAllEventsWithCustomerJSON(&customerList);
        res.set_content(json, "application/json");
    });

    // ==================== START SERVER ====================
    cout << "========================================" << endl;
    cout << "  Afsanay Event Atelier Server" << endl;
    cout << "========================================" << endl;
    cout << "Server starting on http://localhost:8080" << endl;
    cout << "Press Ctrl+C to stop the server" << endl;
    cout << "========================================" << endl;
    
    // Start the server and listen on localhost port 8080
    if (!svr.listen("127.0.0.1", 8080)) {
        // If server fails to start, print error message
        cerr << "Error: Failed to start server on port 8080" << endl;
        return 1;  // Exit with error code 1
    }

    return 0;  // Exit successfully
}
