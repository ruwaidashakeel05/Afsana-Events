#include "utils.h"
using namespace std;

string parseJSON(string json, string key) {
    size_t pos = json.find("\"" + key + "\"");
    if (pos == string::npos) return "";
    
    pos = json.find(":", pos);
    if (pos == string::npos) return "";
    
    pos++;
    while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
    
    if (pos >= json.length()) return "";
    
    if (json[pos] == '"') {
        pos++;
        size_t endPos = json.find("\"", pos);
        if (endPos == string::npos) return "";
        return json.substr(pos, endPos - pos);
    }
    
    size_t endPos = pos;
    while (endPos < json.length() && json[endPos] != ',' && json[endPos] != '}') {
        endPos++;
    }
    return json.substr(pos, endPos - pos);
}

int parseJSONInt(string json, string key) {
    string value = parseJSON(json, key);
    if (value.empty()) return 0;
    try {
        return stoi(value);
    } catch(...) {
        return 0;
    }
}

// JSON Escaping function
string escapeJSON(const string& str) {
    string result;
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c;
        }
    }
    return result;
}
