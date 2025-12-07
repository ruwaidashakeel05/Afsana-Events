// This file is kept for reference but is not compiled
// The main server implementation is in main.cpp
// 
// To compile the project, use:
// g++ main.cpp customer.cpp event.cpp staff.cpp review.cpp task.cpp supervisor.cpp utils.cpp -o server -pthread -std=c++11 -lws2_32

/*
#define _WIN32_WINNT 0x0A00
#define CPPHTTPLIB_DISABLE_MMAP
#define CPPHTTPLIB_WINDOWS_MINGW
#define CPPHTTPLIB_THREAD_POOL_COUNT 8
#include "httplib.h"
using namespace httplib;

int main() {
    Server svr;

    svr.Get("/", [](const Request&, Response& res) {
        res.set_content("Hello from C++ server!", "text/plain");
    });

    svr.listen("0.0.0.0", 8080);
}
*/
