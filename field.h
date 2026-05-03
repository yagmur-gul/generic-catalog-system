#ifndef FIELD_H
#define FIELD_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

// Structure to store field format information
struct FieldFormat{
    string name;      // Field name
    string type;      // Data type (string, integer, double, bool)
    string fieldType; // Field type (single or multi)
};

// Abstract base class for all field types
class FieldBase{
public:
    virtual ~FieldBase()=default;                                      // Virtual destructor
    virtual string getType()const = 0;                                 // Get field type (single/multi)
    virtual string toString()const = 0;                                // Convert to string representation
    virtual bool match(const string& value)const = 0;                 // Check if field matches search value
};

// Template class for single-value fields
template<typename T>
class FieldSingle:public FieldBase{
public:
    FieldSingle(T value):data(value){}                                 // Constructor with initial value
    T getData()const{return data;}                                     // Get stored data
    string getType()const override{return "single";}                   // Return field type
    string toString()const override;                                   // Convert to string
    bool match(const string& value)const override;                     // Match search value

private:
    T data; // Stored data value
};

// Template class for multi-value fields
template<typename T>
class FieldMulti:public FieldBase{
public:
    FieldMulti(const vector<T>& values):data(values){}                 // Constructor with vector of values
    const vector<T>& getData()const{return data;}                     // Get stored data vector
    string getType()const override{return "multi";}                   // Return field type
    string toString()const override;                                   // Convert to string
    bool match(const string& value)const override;                     // Match search value
private:
    vector<T> data; // Vector of stored values
};

// Convert single field value to string
template<typename T>
string FieldSingle<T>::toString()const{
    stringstream ss;
    ss<<data; // Stream data to string
    return ss.str();
}

// Check if single field matches search value using case-insensitive substring search
template<typename T>
bool FieldSingle<T>::match(const string& value)const{
    stringstream ss;
    ss<<data; // Convert data to string
    string dataStr=ss.str();
    string lowerDataStr = dataStr;
    string lowerValue = value;
    transform(lowerDataStr.begin(), lowerDataStr.end(), lowerDataStr.begin(), ::tolower); // Convert to lowercase
    transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);       // Convert to lowercase
    return lowerDataStr.find(lowerValue)!=string::npos; // Check if value found in data
}

// Specialization for bool single field toString
template<>
inline string FieldSingle<bool>::toString()const{
    return data ? "true" : "false"; // Return "true" or "false" string
}

// Specialization for bool single field match
template<>
inline bool FieldSingle<bool>::match(const string& value)const{
    string dataStr = data ? "true" : "false"; // Convert bool to string
    string lowerDataStr = dataStr;
    string lowerValue = value;
    transform(lowerDataStr.begin(), lowerDataStr.end(), lowerDataStr.begin(), ::tolower); // Convert to lowercase
    transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);       // Convert to lowercase
    return lowerDataStr.find(lowerValue)!=string::npos; // Check substring match
}

// Convert multi field values to colon-separated string
template<typename T>
string FieldMulti<T>::toString()const{
    if(data.empty()) {return "";} // Return empty string if no data

    stringstream ss;
    ss<<data[0]; // Add first element
    for(size_t i=1; i<data.size(); i++){
        ss<<":"<<data[i]; // Add remaining elements with colon separator
    }
    return ss.str();
}

// Check if any value in multi field matches search value
template<typename T>
bool FieldMulti<T>::match(const string& value)const{
    if(data.empty()) return false; // Return false if no data
    
    string lowerValue = value;
    transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower); // Convert search value to lowercase
    
    for(const T& item : data){ // Check each item in vector
        std::stringstream ss;
        ss << item; // Convert item to string
        string itemStr = ss.str();
        string lowerItemStr = itemStr;
        transform(lowerItemStr.begin(), lowerItemStr.end(), lowerItemStr.begin(), ::tolower); // Convert to lowercase
        
        // Check for exact match or substring match
        if(lowerItemStr == lowerValue || lowerItemStr.find(lowerValue) != string::npos){
            return true; // Return true if match found
        }
    }
    return false; // Return false if no matches
}

// Specialization for bool multi field toString
template<>
inline string FieldMulti<bool>::toString()const{
    if(data.empty()) {return "";} // Return empty string if no data

    stringstream ss;
    ss << (data[0] ? "true" : "false"); // Add first bool as string
    for(size_t i=1; i<data.size(); i++){
        ss << ":" << (data[i] ? "true" : "false"); // Add remaining bools with colon separator
    }
    return ss.str();
}

// Specialization for bool multi field match
template<>
inline bool FieldMulti<bool>::match(const string& value)const{
    if(data.empty()) return false; // Return false if no data
    
    string lowerValue = value;
    transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower); // Convert search value to lowercase
    
    for(bool item : data){ // Check each bool in vector
        string itemStr = item ? "true" : "false"; // Convert bool to string
        string lowerItemStr = itemStr;
        transform(lowerItemStr.begin(), lowerItemStr.end(), lowerItemStr.begin(), ::tolower); // Convert to lowercase
        
        // Check for exact match or substring match
        if(lowerItemStr == lowerValue || lowerItemStr.find(lowerValue) != string::npos){
            return true; // Return true if match found
        }
    }
    return false; // Return false if no matches
}

#endif