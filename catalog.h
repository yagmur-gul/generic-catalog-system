#ifndef CATALOG_H
#define CATALOG_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "field.h"

using namespace std;

/* Entry class represents a single record in the catalog */
class Entry {
public:
    // Default constructor
    Entry() = default;
    
    // Destructor - cleans up dynamically allocated field objects
    ~Entry();
    
    // Copy constructor - performs deep copy of all fields
    Entry(const Entry& other);
    
    // Assignment operator - performs deep copy with proper cleanup
    Entry& operator=(const Entry& other);
    
    // Add a new field to this entry
    void addField(FieldBase* field);
    
    // Get a field at specified index (returns nullptr if index is out of bounds)
    const FieldBase* getField(size_t index) const;
    
    // Get the total number of fields in this entry
    int getFieldCount() const { return fields.size(); }
    
    // Convert the entire entry to string format (fields separated by |)
    string toString() const;
    
    // Check if any field matches the given value and field name
    bool match(const string& value, const string& fieldName, const vector<FieldFormat>& formats) const;

private:
    // Vector of pointers to field objects (polymorphic storage)
    vector<FieldBase*> fields;
};

/* Catalog class handles reading data, processing commands, and maintaining log files */
class Catalog {
public:
    // Constructor - opens log file for writing
    Catalog(const string& logFileName);
    
    // Destructor - closes log file if still open
    ~Catalog();
    
    // Read and parse data from input file
    void readData(const string& filename);
    
    // Process commands from command file
    void processCommands(const string& filename);
    
    // Search for entries matching a value in a specific field
    void search(const string& value, const string& fieldName);
    
    // Sort all entries by a specific field
    void sort(const string& fieldName);

private:
    // Field format definitions (name, type, fieldType)
    vector<FieldFormat> fieldFormats;
    
    // All catalog entries
    vector<Entry> entries;
    
    // Output log file stream
    ofstream logFile;
    
    // Parse the format line (first line of data file)
    void parseFormat(const string& formatLine);
    
    // Parse a single entry line and add to catalog
    void parseEntry(const string& entryLine);
    
    // Log exception messages to output file
    void logException(const string& message, const string& entryLine = "");
    
    // Log command to output file
    void logCommand(const string& command);
    
    // Log output/results to file
    void logOutput(const string& output);
    
    // Bubble sort implementation for sorting entries
    void bubbleSort(size_t fieldIndex);
    
    // Split string by delimiter and return vector of tokens
    vector<string> splitString(const string& str, char delimiter) const;
    
    // Find index of field by name (returns -1 if not found)
    int findFieldIndex(const string& fieldName) const;
    
    // Template function to parse single value from string
    template<typename T>
    T parseValue(const string& value);
    
    // Template function to parse multiple values from colon-separated string
    template<typename T>
    vector<T> parseMultiValue(const string& value);
};

/*Template function using stringstream to convert string to target type*/
template<typename T>
T Catalog::parseValue(const string& value) {
    T result;
    std::stringstream iss(value);
    iss >> result;
    return result;
}

/* Template function splits by colon and converts each part to target type */
template<typename T>
vector<T> Catalog::parseMultiValue(const string& value) {
    vector<T> results;
    vector<std::string> parts = splitString(value, ':');
    for (size_t i = 0; i < parts.size(); i++) {
        if (!parts[i].empty()) {
            results.push_back(parseValue<T>(parts[i]));
        }
    }
    return results;
}

// Thrown when an entry has missing required fields
class MissingFieldException : public runtime_error {
public:
    MissingFieldException() : runtime_error("Missing field") {}
};

// Thrown when trying to add a duplicate entry (based on first field)
class DuplicateEntryException : public runtime_error {
public:
    DuplicateEntryException() : runtime_error("Duplicate entry") {}
};

// Thrown when command syntax is incorrect
class WrongCommandException : public runtime_error {
public:
    WrongCommandException() : runtime_error("Command is wrong") {}
};

#endif