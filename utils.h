#ifndef UTILS_H
#define UTILS_H

#include <string>
using namespace std;

// ==================== JSON PARSING FUNCTIONS ====================
string parseJSON(string json, string key);
int parseJSONInt(string json, string key);

#endif