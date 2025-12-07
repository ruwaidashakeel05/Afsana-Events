# Afsanay Event Atelier

A full-stack event management application built with C++ backend (REST API) and vanilla HTML/CSS/JavaScript frontend.

## Features

- **Customer Registration & Authentication** — Sign up with email/password, persistent login via token-based sessions
- **Event Management** — Create, edit, delete, and view personal events
- **Event History** — Track completed and past events
- **Staff Reviews** — Rate and review staff members
- **Customer Dashboard** — Personalized welcome, event tracking, notifications
- **Data Persistence** — Customer and event data saved to local text files

## Tech Stack

- **Backend**: C++ with cpp-httplib for HTTP server
- **Frontend**: HTML5, CSS3, Vanilla JavaScript
- **Storage**: Text files (customers.txt, events.txt)
- **Session Management**: Token-based (localStorage)

## Prerequisites

### Windows
- **C++ Compiler**: MinGW/G++ (or any C++11 compatible compiler)
- **Port 8080** available (configurable in main.cpp)

### macOS/Linux
- **G++** or **Clang++** with C++11 support
- **Port 8080** available

## Installation & Setup

### 1. Clone the Repository
```bash
git clone https://github.com/ruwaidashakeel05/Afsana-Events.git
cd Afsana-Events
```

### 2. Compile the Server

#### Windows (PowerShell)
```powershell
cd c:\path\to\EventManagement
g++ main.cpp customer.cpp event.cpp staff.cpp review.cpp task.cpp supervisor.cpp utils.cpp -o server -pthread -std=c++11 -lws2_32
```

#### macOS/Linux
```bash
g++ main.cpp customer.cpp event.cpp staff.cpp review.cpp task.cpp supervisor.cpp utils.cpp -o server -pthread -std=c++11
```

### 3. Run the Server
```bash
./server
```

Expected output:
```
Server started on 0.0.0.0:8080
========================================
Endpoints Available:
  POST   /api/auth/register
  POST   /api/auth/login
  POST   /api/auth/logout
  GET    /api/customer/profile
  PUT    /api/customer/profile
  GET    /api/events
  POST   /api/events
  PUT    /api/events/:id
  DELETE /api/events/:id
  GET    /api/history
  GET    /api/staff
  POST   /api/reviews
  GET    /api/notifications
========================================
```

### 4. Open in Browser
```
http://localhost:8080
```

## Usage

### Sign Up (Customer)
1. Go to **Login** page
2. Click **Sign Up** tab
3. Enter: Name, Email, Password, Phone, Address
4. Click **Sign Up**
5. Redirected to **Customer Dashboard** (logged in automatically)

### Log In (Customer)
1. Go to **Login** page
2. Enter Email and Password
3. Click **Login**
4. Redirected to **Customer Dashboard**

### Customer Dashboard
- **Welcome Message**: Shows your name (e.g., "Welcome Ruwaida")
- **My Events Tab**: View, edit, delete personal events
- **Event History Tab**: View completed/cancelled events
- **Add New Event**: Click button to create event with details (name, type, date, time, location, guests, description)
- **Logout**: Click logout button in top-right

## Project Structure

```
EventManagement/
├── main.cpp                 # HTTP server & API endpoints
├── customer.cpp/h           # Customer management (register, login, profile)
├── event.cpp/h              # Event CRUD operations
├── staff.cpp/h              # Staff management
├── review.cpp/h             # Staff review system
├── notification.h           # Notification queue
├── session.h                # Session/token management
├── eventstack.h             # Event history stack
├── task.cpp/h               # Task assignment
├── supervisor.cpp/h         # Supervisor operations
├── utils.cpp/h              # Utility functions
│
├── index.html               # Home page
├── login.html               # Login/Signup page
├── customer.html            # Customer dashboard (protected)
├── about.html               # About page
├── contact.html             # Contact page
├── admin.html               # Admin page
├── supervisor.html          # Supervisor page
├── staff.html               # Staff page
│
├── images/                  # Logo and images
├── customers.txt            # Customer data (auto-created)
├── events.txt               # Event data (auto-created)
│
└── httplib.h                # Single-header HTTP library
```

## Data Files

- **customers.txt**: Format: `id|name|email|password|phone|address`
- **events.txt**: Format: `id|customerId|name|type|date|time|location|address|guests|description|status`

Files are created automatically when you register the first customer or add the first event.

## API Endpoints

### Authentication
- `POST /api/auth/register` — Register new customer
- `POST /api/auth/login` — Login customer
- `POST /api/auth/logout` — Logout customer

### Customer Profile
- `GET /api/customer/profile` — Get current customer profile (requires token)
- `PUT /api/customer/profile` — Update customer profile (requires token)

### Events
- `GET /api/events` — Get customer's active events (requires token)
- `POST /api/events` — Create new event (requires token)
- `PUT /api/events/:id` — Update event (requires token)
- `DELETE /api/events/:id` — Delete/archive event (requires token)
- `GET /api/history` — Get completed/cancelled events (requires token)

### Staff & Reviews
- `GET /api/staff` — Get all staff members
- `POST /api/reviews` — Submit staff review (requires token)

### Frontend Pages
- `GET /` → index.html
- `GET /login` → login.html
- `GET /customer.html` → customer dashboard
- `GET /about` → about.html
- `GET /contact` → contact.html
- `GET /images/*` → serve images

## Troubleshooting

### Server won't compile
- Ensure C++11 is enabled: add `-std=c++11` flag
- Windows users: add `-lws2_32` for socket library
- Check that all `.cpp` and `.h` files are in the same directory

### "Port 8080 already in use"
- Close other apps using port 8080 or edit `main.cpp` line ~600 to change port:
  ```cpp
  svr.listen("0.0.0.0", 8080);  // Change 8080 to another port
  ```

### "Unauthorized" when saving events
- Ensure you're logged in (check localStorage key `af_token`)
- Server rejects requests without valid Authorization header

### Customer data not persisting
- Check that `customers.txt` and `events.txt` are in the working directory
- Ensure write permissions on the project folder

## Browser Support

- Chrome/Edge (latest)
- Firefox (latest)
- Safari (latest)

## Future Enhancements

- [ ] Staff/Admin authentication and dashboards
- [ ] Email notifications
- [ ] Calendar view for events
- [ ] Payment integration
- [ ] Database migration (replace text files with SQLite/PostgreSQL)
- [ ] Mobile app (React Native/Flutter)

## License

This project is part of the Afsanay Event Atelier portfolio.

## Contact & Support

- **Email**: info@afsanayevents.com
- **GitHub**: https://github.com/ruwaidashakeel05/Afsana-Events

---

**Built with ❤️ by ARK Team**
