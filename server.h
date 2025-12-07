#ifndef SERVER_H
#define SERVER_H

#include "customer.h"
#include "staffmanager.h"
#include "utils.h"
#include <string>
#include <iostream>
using namespace std;

// Request handler function
string handleRequest(string method, string path, string body);

#endif