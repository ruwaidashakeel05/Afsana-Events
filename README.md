# Event Management System

A comprehensive web-based event management application built with C++ and HTTP server technology, featuring role-based access control for customers, staff, supervisors, and administrators.

## Overview

This Event Management System is a full-featured platform that allows users to create, manage, and track events. It implements data structures and algorithms for efficient management of customers, staff, supervisors, tasks, and event histories.

## Features

### 🎯 Core Functionality
- **Event Management**: Create, view, and manage events with detailed information
- **Customer Management**: Register and manage customer profiles and event bookings
- **Staff Management**: Manage staff members and their assignments
- **Supervisor Management**: Supervisors can accept and oversee events
- **Task Assignment**: Assign and track tasks related to events
- **Event History**: Stack-based tracking of event history

### 👥 User Roles
- **Customer**: Browse events, create bookings, manage reservations
- **Staff**: View assigned tasks and event details
- **Supervisor**: Accept events, manage staff assignments, oversee event execution
- **Admin**: System administration, user management, and reporting

### 📝 Key Features by Role
- Validation for phone numbers (11 digits, starts with 03)
- Email validation
- Session management and authentication
- Persistent data storage (text files)
- HTTP-based web interface

## Project Structure

```
EventManagement/
├── main.cpp              # Main application entry point with HTTP server setup
├── customer.h/cpp        # Customer management (linked list structure)
├── event.h/cpp           # Event management (linked list structure)
├── staff.h/cpp           # Staff member management
├── supervisor.h/cpp      # Supervisor management and event assignments
├── task.h/cpp            # Task assignment and tracking
├── eventstack.h          # Event history using stack data structure
├── utils.h/cpp           # Utility functions for file I/O and validation
├── httplib.h             # HTTP server library (for web interface)
├── index.html            # Home page
├── login.html            # Login page
├── admin.html            # Admin dashboard
├── customer.html         # Customer portal
├── staff.html            # Staff dashboard
├── supervisor.html       # Supervisor dashboard
├── contact.html          # Contact page
├── about.html            # About page
├── customers.txt         # Customer data persistence
├── events.txt            # Event data persistence
├── staff.txt             # Staff data persistence
├── supervisor.txt        # Supervisor data persistence
├── tasks.txt             # Task assignment data persistence
└── images/               # Image assets
```

## Data Structures

### Linked Lists
- **CustomerList**: Manages customer records with linked list implementation
- **EventList**: Tracks all events in a linked list
- **StaffList**: Manages staff members
- **SupervisorList**: Manages supervisor accounts

### Stack
- **EventStack**: Maintains event history using Last-In-First-Out (LIFO) structure

### Maps
- **activeSessions**: Session tracking for authenticated users
- **sessionRoles**: Maps session IDs to user roles

## Technical Stack

- **Language**: C++
- **Web Server**: HTTP Library (cpp-httplib)
- **Data Storage**: Text-based file system
- **Architecture**: Linked Lists, Stacks, Maps

## Getting Started

### Prerequisites
- C++ compiler (Visual Studio, GCC, or Clang)
- Windows 10 or later (for Windows API support)

### Building

1. Compile the main application:
```bash
g++ -std=c++17 main.cpp customer.cpp event.cpp staff.cpp supervisor.cpp task.cpp utils.cpp -o event_management
```

2. Run the application:
```bash
./event_management
```

### Running the Web Interface

Once the server is running, access the web interface at:
- **Home**: `http://localhost:8080/`
- **Login**: `http://localhost:8080/login`
- **Admin**: `http://localhost:8080/admin`

## API Endpoints

The system provides HTTP endpoints for:
- User authentication and session management
- Event CRUD operations
- Customer profile management
- Staff assignment and task tracking
- Supervisor event acceptance and management

## Data Validation

- **Phone Numbers**: Must be 11 digits and start with "03"
- **Email**: Must contain @ and . with valid format
- **Passwords**: User-defined with storage in data files

## File Format

Data is stored in text files (`.txt`) with pipe-delimited (`|`) format:
- Each line represents a record
- Fields are separated by pipes
- Easy to parse and manipulate

## Session Management

- Session IDs are generated sequentially starting from 1000
- Active sessions are tracked in memory
- User roles are associated with each session

## Future Enhancements

- Database integration (MySQL/PostgreSQL)
- Email notifications
- Advanced reporting and analytics
- Mobile application
- Real-time event updates
- Payment integration

## License

This project is part of the DSA (Data Structures and Algorithms) course project.

## Contributors

Created as a comprehensive exercise in data structures, algorithms, and web application development.

---

**Note**: This application uses text file-based storage. For production use, consider migrating to a proper database system.
