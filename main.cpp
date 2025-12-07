#define _WIN32_WINNT 0x0A00
#define CPPHTTPLIB_DISABLE_MMAP
#define CPPHTTPLIB_WINDOWS_MINGW

#include "event.h"
#include "customer.h"
#include "staff.h"
#include "review.h"
#include "notification.h"
#include "session.h"
#include "eventstack.h"
#include "httplib.h"
#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <sstream>

using namespace std;
using namespace httplib;

// Global data structures
EventList eventList("events.txt");
StaffList staffList;
ReviewList reviewList;
EventStack eventHistory;
CustomerList customerList("customers.txt");
NotificationQueue notificationQueue;
SessionStack sessionStack;

// Helper function to parse JSON (simple parser for our needs)
string getJsonValue(const string& json, const string& key) {
    size_t pos = json.find("\"" + key + "\"");
    if (pos == string::npos) return "";
    
    pos = json.find(":", pos);
    if (pos == string::npos) return "";
    
    pos = json.find("\"", pos);
    if (pos == string::npos) return "";
    
    size_t endPos = json.find("\"", pos + 1);
    if (endPos == string::npos) return "";
    
    return json.substr(pos + 1, endPos - pos - 1);
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

// Helper function to generate session token
string generateToken(int customerId) {
    srand(time(0) + customerId);
    string token = "TOKEN_";
    for (int i = 0; i < 16; i++) {
        token += char('A' + rand() % 26);
    }
    return token;
}

// Helper function to get current date/time
string getCurrentDateTime() {
    time_t now = time(0);
    char buffer[80];
    struct tm* timeinfo = localtime(&now);
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
    return string(buffer);
}

int main() {
    Server svr;

    // Enable CORS
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type, Authorization"}
    });

    // Handle OPTIONS requests for CORS
    svr.Options("/(.*)", [](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
    });

    // ==================== FRONTEND ROUTES ====================
    
    // Serve index.html as root
    svr.Get("/", [](const Request&, Response& res) {
        ifstream file("index.html");
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
            file.close();
        } else {
            res.status = 404;
            res.set_content("index.html not found", "text/plain");
        }
    });

    // Serve login page
    svr.Get("/login", [](const Request&, Response& res) {
        ifstream file("login.html");
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
            file.close();
        } else {
            res.status = 404;
            res.set_content("login.html not found", "text/plain");
        }
    });

    // Serve registration page
    svr.Get("/register", [](const Request&, Response& res) {
        ifstream file("customer.html");
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
            file.close();
        } else {
            res.status = 404;
            res.set_content("customer.html not found", "text/plain");
        }
    });

    // Serve contact page
    svr.Get("/contact", [](const Request&, Response& res) {
        ifstream file("contact.html");
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
            file.close();
        } else {
            res.status = 404;
            res.set_content("contact.html not found", "text/plain");
        }
    });

    // Serve about page
    svr.Get("/about", [](const Request&, Response& res) {
        ifstream file("about.html");
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
            file.close();
        } else {
            res.status = 404;
            res.set_content("about.html not found", "text/plain");
        }
    });

    // Serve staff page
    svr.Get("/staff", [](const Request&, Response& res) {
        ifstream file("staff.html");
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
            file.close();
        } else {
            res.status = 404;
            res.set_content("staff.html not found", "text/plain");
        }
    });

    // Serve admin page
    svr.Get("/admin", [](const Request&, Response& res) {
        ifstream file("admin.html");
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
            file.close();
        } else {
            res.status = 404;
            res.set_content("admin.html not found", "text/plain");
        }
    });

    // Serve supervisor page
    svr.Get("/supervisor", [](const Request&, Response& res) {
        ifstream file("supervisor.html");
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
            file.close();
        } else {
            res.status = 404;
            res.set_content("supervisor.html not found", "text/plain");
        }
    });

    // ==================== STATIC FILES (Images, CSS, JS) ====================
    
    // Serve images
    svr.Get("/images/(.*)", [](const Request& req, Response& res) {
        string filename = req.matches[1];
        string filepath = "images/" + filename;
        
        ifstream file(filepath, ios::binary);
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf();
            
            // Set content type based on file extension
            string content_type = "application/octet-stream";
            if (filename.find(".jpg") != string::npos || filename.find(".jpeg") != string::npos) {
                content_type = "image/jpeg";
            } else if (filename.find(".png") != string::npos) {
                content_type = "image/png";
            } else if (filename.find(".gif") != string::npos) {
                content_type = "image/gif";
            } else if (filename.find(".css") != string::npos) {
                content_type = "text/css";
            } else if (filename.find(".js") != string::npos) {
                content_type = "text/javascript";
            }
            
            res.set_content(buffer.str(), content_type.c_str());
            file.close();
        } else {
            res.status = 404;
            res.set_content("File not found", "text/plain");
        }
    });

    // Serve HTML files directly by name (e.g., /about.html)
    svr.Get("/(.*\\.html)", [](const Request& req, Response& res) {
        string filename = req.matches[1];
        ifstream file(filename);
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
            file.close();
        } else {
            res.status = 404;
            res.set_content("File not found", "text/plain");
        }
    });

    // ==================== CUSTOMER AUTHENTICATION ====================
    
    // Register new customer
    svr.Post("/api/auth/register", [](const Request& req, Response& res) {
        string body = req.body;
        
        string name = getJsonValue(body, "name");
        string email = getJsonValue(body, "email");
        string password = getJsonValue(body, "password");
        string phone = getJsonValue(body, "phone");
        string address = getJsonValue(body, "address");
        
        int customerId = customerList.registerCustomer(name, email, password, phone, address);
        
        string response;
        if (customerId == -1) {
            response = "{\"success\":false,\"message\":\"Email already exists\"}";
        } else {
            // Generate session token
            string token = generateToken(customerId);
            string loginTime = getCurrentDateTime();
            sessionStack.push(customerId, token, loginTime);
            
            CustomerNode* customer = customerList.findById(customerId);
            response = "{\"success\":true,\"message\":\"Registration successful\",";
            response += "\"token\":\"" + token + "\",";
            response += "\"customer\":" + customerList.getCustomerJSON(customer) + "}";
            
            // Send welcome notification
            notificationQueue.enqueue(customerId, "Welcome to Afsanay Event Atelier!", getCurrentDateTime());
        }
        res.set_content(response, "application/json");
    });
    
    // Login customer
    svr.Post("/api/auth/login", [](const Request& req, Response& res) {
        string body = req.body;
        
        string email = getJsonValue(body, "email");
        string password = getJsonValue(body, "password");
        
        CustomerNode* customer = customerList.login(email, password);
        
        string response;
        if (customer == nullptr) {
            response = "{\"success\":false,\"message\":\"Invalid email or password\"}";
        } else {
            // Generate session token
            string token = generateToken(customer->id);
            string loginTime = getCurrentDateTime();
            sessionStack.push(customer->id, token, loginTime);
            
            response = "{\"success\":true,\"message\":\"Login successful\",";
            response += "\"token\":\"" + token + "\",";
            response += "\"customer\":" + customerList.getCustomerJSON(customer) + "}";
        }
        res.set_content(response, "application/json");
    });
    
    // Logout customer
    svr.Post("/api/auth/logout", [](const Request& req, Response& res) {
        string token = req.get_header_value("Authorization");
        
        bool success = sessionStack.removeSession(token);
        
        string response;
        if (success) {
            response = "{\"success\":true,\"message\":\"Logout successful\"}";
        } else {
            response = "{\"success\":false,\"message\":\"Invalid session\"}";
        }
        res.set_content(response, "application/json");
    });

    // ==================== CUSTOMER PROFILE ====================
    
    // Get customer profile
    svr.Get("/api/customer/profile", [](const Request& req, Response& res) {
        string token = req.get_header_value("Authorization");
        int customerId = sessionStack.findCustomerByToken(token);
        
        if (customerId == -1) {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        CustomerNode* customer = customerList.findById(customerId);
        string response = "{\"success\":true,\"customer\":" + customerList.getCustomerJSON(customer) + "}";
        res.set_content(response, "application/json");
    });
    
    // Update customer profile
    svr.Put("/api/customer/profile", [](const Request& req, Response& res) {
        string token = req.get_header_value("Authorization");
        int customerId = sessionStack.findCustomerByToken(token);
        
        if (customerId == -1) {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        string body = req.body;
        string name = getJsonValue(body, "name");
        string phone = getJsonValue(body, "phone");
        string address = getJsonValue(body, "address");
        
        bool success = customerList.updateCustomer(customerId, name, phone, address);
        
        string response;
        if (success) {
            response = "{\"success\":true,\"message\":\"Profile updated successfully\"}";
        } else {
            response = "{\"success\":false,\"message\":\"Failed to update profile\"}";
        }
        res.set_content(response, "application/json");
    });

    // ==================== EVENTS ====================
    
    // Get all events (for customer - filtered by their ID)
    svr.Get("/api/events", [](const Request& req, Response& res) {
        string token = req.get_header_value("Authorization");
        int customerId = sessionStack.findCustomerByToken(token);
        
        string json;
        if (customerId != -1) {
            // Return only customer's events
            json = eventList.getCustomerEventsJSON(customerId);
        } else {
            // If no auth, return empty array
            json = "[]";
        }
        res.set_content(json, "application/json");
    });

    // Add new event
    svr.Post("/api/events", [](const Request& req, Response& res) {
        string token = req.get_header_value("Authorization");
        int customerId = sessionStack.findCustomerByToken(token);
        
        if (customerId == -1) {
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
        
        int id = eventList.addEvent(customerId, name, type, date, time, location, address, guests, description);
        
        // Send notification
        notificationQueue.enqueue(customerId, "Your event '" + name + "' has been created successfully!", getCurrentDateTime());
        
        string response = "{\"success\":true,\"message\":\"Event added successfully\",\"id\":" + to_string(id) + "}";
        res.set_content(response, "application/json");
    });

    // Update event
    svr.Put("/api/events/(\\d+)", [](const Request& req, Response& res) {
        string token = req.get_header_value("Authorization");
        int customerId = sessionStack.findCustomerByToken(token);
        
        if (customerId == -1) {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        int id = stoi(req.matches[1]);
        
        // Verify event belongs to customer
        EventNode* event = eventList.getEvent(id);
        if (event == nullptr || event->customerId != customerId) {
            res.set_content("{\"success\":false,\"message\":\"Event not found or unauthorized\"}", "application/json");
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
        
        bool success = eventList.updateEvent(id, name, type, date, time, location, address, guests, description);
        
        string response;
        if (success) {
            notificationQueue.enqueue(customerId, "Your event '" + name + "' has been updated!", getCurrentDateTime());
            response = "{\"success\":true,\"message\":\"Event updated successfully\"}";
        } else {
            response = "{\"success\":false,\"message\":\"Event not found\"}";
        }
        res.set_content(response, "application/json");
    });

    // Delete event
    svr.Delete("/api/events/(\\d+)", [](const Request& req, Response& res) {
        string token = req.get_header_value("Authorization");
        int customerId = sessionStack.findCustomerByToken(token);
        
        if (customerId == -1) {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        int id = stoi(req.matches[1]);
        
        // Get event before deleting
        EventNode* event = eventList.getEvent(id);
        if (event == nullptr || event->customerId != customerId) {
            res.set_content("{\"success\":false,\"message\":\"Event not found or unauthorized\"}", "application/json");
            return;
        }
        
        // Create a copy for history
        EventNode* historyCopy = new EventNode(
            event->id, event->customerId, event->name, event->type, event->date, event->time,
            event->location, event->address, event->guests, event->description, "completed"
        );
        eventHistory.push(historyCopy);
        
        bool success = eventList.deleteEvent(id);
        
        string response;
        if (success) {
            notificationQueue.enqueue(customerId, "Your event '" + event->name + "' has been moved to history!", getCurrentDateTime());
            response = "{\"success\":true,\"message\":\"Event deleted successfully\"}";
        } else {
            response = "{\"success\":false,\"message\":\"Event not found\"}";
        }
        res.set_content(response, "application/json");
    });

    // Get event history
    svr.Get("/api/history", [](const Request& req, Response& res) {
        string json = eventHistory.getAllHistoryJSON();
        res.set_content(json, "application/json");
    });

    // ==================== STAFF ====================
    
    // Get all staff
    svr.Get("/api/staff", [](const Request& req, Response& res) {
        string json = staffList.getAllStaffJSON();
        res.set_content(json, "application/json");
    });

    // ==================== REVIEWS ====================
    
    // Add review
    svr.Post("/api/reviews", [](const Request& req, Response& res) {
        string token = req.get_header_value("Authorization");
        int customerId = sessionStack.findCustomerByToken(token);
        
        if (customerId == -1) {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        string body = req.body;
        
        int staffId = getJsonInt(body, "staffId");
        int rating = getJsonInt(body, "rating");
        string comment = getJsonValue(body, "comment");
        
        reviewList.addReview(staffId, rating, comment);
        
        string response = "{\"success\":true,\"message\":\"Review submitted successfully\"}";
        res.set_content(response, "application/json");
    });

    // ==================== NOTIFICATIONS ====================
    
    // Get customer notifications
    svr.Get("/api/notifications", [](const Request& req, Response& res) {
        string token = req.get_header_value("Authorization");
        int customerId = sessionStack.findCustomerByToken(token);
        
        if (customerId == -1) {
            res.set_content("{\"success\":false,\"message\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        string json = notificationQueue.getCustomerNotificationsJSON(customerId);
        res.set_content(json, "application/json");
    });
    
    // Mark notification as read
    svr.Put("/api/notifications/(\\d+)/read", [](const Request& req, Response& res) {
        int notificationId = stoi(req.matches[1]);
        
        bool success = notificationQueue.markAsRead(notificationId);
        
        string response;
        if (success) {
            response = "{\"success\":true,\"message\":\"Notification marked as read\"}";
        } else {
            response = "{\"success\":false,\"message\":\"Notification not found\"}";
        }
        res.set_content(response, "application/json");
    });

    // ==================== START SERVER ====================
    cout << "========================================" << endl;
    cout << "  Afsanay Event Atelier Server" << endl;
    cout << "========================================" << endl;
    cout << "Server starting on http://localhost:8080" << endl;
    cout << "Press Ctrl+C to stop the server" << endl;
    cout << "========================================" << endl;
    cout << "\nEndpoints Available:" << endl;
    cout << "  POST   /api/auth/register" << endl;
    cout << "  POST   /api/auth/login" << endl;
    cout << "  POST   /api/auth/logout" << endl;
    cout << "  GET    /api/customer/profile" << endl;
    cout << "  PUT    /api/customer/profile" << endl;
    cout << "  GET    /api/events" << endl;
    cout << "  POST   /api/events" << endl;
    cout << "  PUT    /api/events/:id" << endl;
    cout << "  DELETE /api/events/:id" << endl;
    cout << "  GET    /api/history" << endl;
    cout << "  GET    /api/staff" << endl;
    cout << "  POST   /api/reviews" << endl;
    cout << "  GET    /api/notifications" << endl;
    cout << "========================================" << endl;
    
    if (!svr.listen("0.0.0.0", 8080)) {
        cerr << "Error: Failed to start server on port 8080" << endl;
        cerr << "Make sure the port is not already in use." << endl;
        return 1;
    }

    return 0;
}