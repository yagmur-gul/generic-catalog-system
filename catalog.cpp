#include "catalog.h"

using namespace std;

// Destructor: Clean up dynamically allocated field objects
Entry::~Entry(){
    for(size_t i = 0; i < fields.size(); i++){
        delete fields[i]; // Delete each field to prevent memory leaks
    }
}

// Copy constructor: Deep copy of Entry object
Entry::Entry(const Entry& other){
    for(size_t i = 0; i < other.fields.size(); i++){
        if(other.fields[i]->getType() == "single"){ // Handle single-value fields
            if(const FieldSingle<string>* f = dynamic_cast<const FieldSingle<string>*>(other.fields[i])){
                fields.push_back(new FieldSingle<string>(f->getData()));
            }else if(const FieldSingle<int>* f = dynamic_cast<const FieldSingle<int>*>(other.fields[i])){
                fields.push_back(new FieldSingle<int>(f->getData()));
            }else if(const FieldSingle<double>* f = dynamic_cast<const FieldSingle<double>*>(other.fields[i])){
                fields.push_back(new FieldSingle<double>(f->getData()));
            }else if(const FieldSingle<bool>* f = dynamic_cast<const FieldSingle<bool>*>(other.fields[i])){
                fields.push_back(new FieldSingle<bool>(f->getData()));
            }
        }else if(other.fields[i]->getType() == "multi"){ // Handle multi-value fields
            if(const FieldMulti<string>* f = dynamic_cast<const FieldMulti<string>*>(other.fields[i])){
                fields.push_back(new FieldMulti<string>(f->getData()));
            }else if(const FieldMulti<int>* f = dynamic_cast<const FieldMulti<int>*>(other.fields[i])){
                fields.push_back(new FieldMulti<int>(f->getData()));
            }else if(const FieldMulti<double>* f = dynamic_cast<const FieldMulti<double>*>(other.fields[i])){
                fields.push_back(new FieldMulti<double>(f->getData()));
            }else if(const FieldMulti<bool>* f = dynamic_cast<const FieldMulti<bool>*>(other.fields[i])){
                fields.push_back(new FieldMulti<bool>(f->getData()));
            }
        }
    }
}

// Assignment operator: Deep copy with self-assignment check
Entry& Entry::operator=(const Entry& other){
    if(this != &other){ // Prevent self-assignment
        for(size_t i = 0; i < fields.size(); i++){
            delete fields[i]; // Clean up existing fields
        }
        fields.clear(); // Clear the vector

        for(size_t i = 0; i < other.fields.size(); i++){
            const FieldBase* field = other.fields[i];
            if(field->getType() == "single"){ // Copy single-value fields
                if(const FieldSingle<string>* f = dynamic_cast<const FieldSingle<string>*>(field)){
                    fields.push_back(new FieldSingle<string>(f->getData()));
                }else if(const FieldSingle<int>* f = dynamic_cast<const FieldSingle<int>*>(field)){
                    fields.push_back(new FieldSingle<int>(f->getData()));
                }else if(const FieldSingle<double>* f = dynamic_cast<const FieldSingle<double>*>(field)){
                    fields.push_back(new FieldSingle<double>(f->getData()));
                }else if(const FieldSingle<bool>* f = dynamic_cast<const FieldSingle<bool>*>(field)){
                    fields.push_back(new FieldSingle<bool>(f->getData()));
                }
            }else{ // Copy multi-value fields
                if(const FieldMulti<string>* f = dynamic_cast<const FieldMulti<string>*>(field)){
                    fields.push_back(new FieldMulti<string>(f->getData()));
                }else if(const FieldMulti<int>* f = dynamic_cast<const FieldMulti<int>*>(field)){
                    fields.push_back(new FieldMulti<int>(f->getData()));
                }else if(const FieldMulti<double>* f = dynamic_cast<const FieldMulti<double>*>(field)){
                    fields.push_back(new FieldMulti<double>(f->getData()));
                }else if(const FieldMulti<bool>* f = dynamic_cast<const FieldMulti<bool>*>(field)){
                    fields.push_back(new FieldMulti<bool>(f->getData()));
                }
            }
        }
    }
    return *this;
}

// Add a field to the entry
void Entry::addField(FieldBase* field){
    fields.push_back(field); // Add field to the vector
}

// Get field at specific index with bounds checking
const FieldBase* Entry::getField(size_t index) const{
    if(index < fields.size()){
        return fields[index]; // Return field if index is valid
    }
    return nullptr; // Return null if index is out of bounds
}

// Convert entry to pipe-separated string representation
string Entry::toString() const{
    if(fields.empty()) return ""; // Return empty string if no fields
    stringstream ss;
    ss << fields[0]->toString(); // Add first field
    for(size_t i = 1; i < fields.size(); i++){
        ss << "|" << fields[i]->toString(); // Add remaining fields with pipe separator
    }
    return ss.str();
}

// Check if entry matches search value in specified field
bool Entry::match(const string& value, const string& fieldName, const vector<FieldFormat>& formats) const{
    for(size_t i = 0; i < formats.size(); i++){
        if(formats[i].name == fieldName && i < fields.size()){ // Find matching field name
            return fields[i]->match(value); // Check if field matches value
        }
    }
    return false; // Return false if field not found
}

// Constructor: Initialize catalog with log file
Catalog::Catalog(const string& logFileName){
    logFile.open(logFileName); // Open log file for writing
    if(!logFile.is_open()){
        throw runtime_error("Cannot open log file: " + logFileName); // Throw error if file can't be opened
    }
}

// Destructor: Close log file if open
Catalog::~Catalog(){
    if(logFile.is_open()){
        logFile.close(); // Close log file
    }
}

// Split string by delimiter into vector of tokens
vector<string> Catalog::splitString(const string& str, char delimiter) const{
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);
    while(getline(tokenStream, token, delimiter)){ // Read tokens separated by delimiter
        tokens.push_back(token); // Add token to vector
    }
    return tokens;
}

// Parse format line to extract field definitions
void Catalog::parseFormat(const string& formatLine){
    vector<string> fieldFormatsStr = splitString(formatLine, '|'); // Split by pipe

    for(size_t i = 0; i < fieldFormatsStr.size(); i++){
        vector<string> parts = splitString(fieldFormatsStr[i], ':'); // Split by colon

        if(parts.size() != 3){continue;} // Skip if not exactly 3 parts

        FieldFormat format;
        format.name = parts[0]; // Field name
        format.type = parts[1]; // Data type
        format.fieldType = parts[2]; // Single or multi

        fieldFormats.push_back(format); // Add to field formats vector
    }

    for(size_t i = 0; i < fieldFormats.size(); i++){
        logFile << fieldFormats[i].name; // Write field name to log
        if(i < fieldFormats.size() - 1){
            logFile << "|"; // Add pipe separator between names
        }
    }
    logFile << endl; // End line in log
}

// Parse entry line and create Entry object with field validation
void Catalog::parseEntry(const string& entryLine){
    vector<string> parts = splitString(entryLine, '|'); // Split entry into parts

    if(parts.size() < fieldFormats.size()){
        throw MissingFieldException(); // Throw if not enough fields
    }

    // Check for duplicate entries by comparing first field
    for(const Entry& existingEntry : entries){
        const FieldBase* firstField = existingEntry.getField(0);
        if(firstField && firstField->toString() == parts[0]){
            throw DuplicateEntryException(); // Throw if duplicate found
        }
    }

    Entry entry; // Create new entry

    for(size_t i = 0; i < fieldFormats.size(); i++){
        if(i >= parts.size() || parts[i].empty()){ // Handle missing or empty fields
            if(fieldFormats[i].fieldType == "single"){ // Create default single field
                if(fieldFormats[i].type == "string"){
                    entry.addField(new FieldSingle<string>(""));
                }else if(fieldFormats[i].type == "integer"){
                    entry.addField(new FieldSingle<int>(0));
                }else if(fieldFormats[i].type == "double"){
                    entry.addField(new FieldSingle<double>(0.0));
                }else if(fieldFormats[i].type == "bool"){
                    entry.addField(new FieldSingle<bool>(false));
                }
            }else{ // Create default multi field
                if(fieldFormats[i].type == "string"){
                    if(i >= parts.size() || parts[i].empty()){
                        entry.addField(new FieldMulti<string>(vector<string>()));
                    }else{
                        entry.addField(new FieldMulti<string>(parseMultiValue<string>(parts[i])));
                    }
                }else if(fieldFormats[i].type == "integer"){
                    entry.addField(new FieldMulti<int>(parseMultiValue<int>(parts[i])));
                }else if(fieldFormats[i].type == "double"){
                    entry.addField(new FieldMulti<double>(parseMultiValue<double>(parts[i])));
                }else if(fieldFormats[i].type == "bool"){
                    entry.addField(new FieldMulti<bool>(parseMultiValue<bool>(parts[i])));
                }
            }
            continue;
        }

        if(fieldFormats[i].fieldType == "single"){ // Create single-value field
            if(fieldFormats[i].type == "string"){
                entry.addField(new FieldSingle<string>(parts[i]));
            }else if(fieldFormats[i].type == "integer"){
                entry.addField(new FieldSingle<int>(parseValue<int>(parts[i])));
            }else if(fieldFormats[i].type == "double"){
                entry.addField(new FieldSingle<double>(parseValue<double>(parts[i])));
            }else if(fieldFormats[i].type == "bool"){
                entry.addField(new FieldSingle<bool>(parseValue<bool>(parts[i])));
            }
        }else{ // Create multi-value field
            if(fieldFormats[i].type == "string"){
                if(parts[i].empty()){
                    entry.addField(new FieldMulti<string>(vector<string>()));
                }else{
                    entry.addField(new FieldMulti<string>(parseMultiValue<string>(parts[i])));
                }
            }else if(fieldFormats[i].type == "integer"){
                entry.addField(new FieldMulti<int>(parseMultiValue<int>(parts[i])));
            }else if(fieldFormats[i].type == "double"){
                entry.addField(new FieldMulti<double>(parseMultiValue<double>(parts[i])));
            }else if(fieldFormats[i].type == "bool"){
                entry.addField(new FieldMulti<bool>(parseMultiValue<bool>(parts[i])));
            }
        }
    }
    entries.push_back(entry); // Add entry to catalog
}

// Find field index by name, return -1 if not found
int Catalog::findFieldIndex(const string& fieldName) const{
    for(size_t i = 0; i < fieldFormats.size(); i++){
        if(fieldFormats[i].name == fieldName){
            return static_cast<int>(i); // Return index if found
        }
    }
    return -1; // Return -1 if not found
}

// Log exception message and optional entry line to log file
void Catalog::logException(const string& message, const string& entryLine){
    logFile << "Exception: " << message << endl; // Write exception message
    if(!entryLine.empty()){
        logFile << entryLine << endl; // Write entry line if provided
    }
}

// Log command to log file
void Catalog::logCommand(const string& command){
    logFile << command << endl; // Write command to log
}

// Log output to log file with newline handling
void Catalog::logOutput(const string& output){
    if(!output.empty()){
        logFile << output; // Write output
        if(output.back() != '\n'){
            logFile << endl; // Add newline if not present
        }
    }
}

// Read data file, parse format and entries
void Catalog::readData(const string& filename){
    ifstream dataFile(filename); // Open data file
    if(!dataFile.is_open()){
        throw runtime_error("Cannot open data file: " + filename); // Throw if can't open
    }
    string line;

    if(getline(dataFile, line)){
        parseFormat(line); // Parse first line as format
    }

    int uniqueEntries = 0; // Counter for successfully added entries
    while(getline(dataFile, line)){
        try{
            parseEntry(line); // Try to parse entry
            uniqueEntries++; // Increment counter on success
        }catch(const MissingFieldException& e){
            logException("missing field", line); // Log missing field error
        }catch(const DuplicateEntryException& e){
            logException("duplicate entry", line); // Log duplicate entry error
        }
    }
    dataFile.close(); // Close data file
    logFile << uniqueEntries << " unique entries" << endl; // Log total unique entries
}
    
// Process commands from command file
void Catalog::processCommands(const string& filename){
    ifstream commandsFile(filename); // Open commands file
    if(!commandsFile.is_open()){
        throw runtime_error("Cannot open command file: " + filename); // Throw if can't open
    }

    string line;
    while(getline(commandsFile, line)){
        logCommand(line); // Log each command

        try{
            istringstream iss(line);
            string command;
            iss >> command; // Extract command type

            if(command == "search"){ // Handle search command
                string value, in, fieldName;
                iss >> value >> in >> fieldName; // Parse search parameters

                value = value.substr(1, value.length() - 2); // Remove quotes from value
                fieldName = fieldName.substr(1, fieldName.length() - 2); // Remove quotes from field name

                if(in != "in"){
                    throw WrongCommandException(); // Throw if syntax is wrong
                }

                search(value, fieldName); // Execute search
            }
            else if(command == "sort"){ // Handle sort command
                string fieldName;
                iss >> fieldName; // Parse field name

                fieldName = fieldName.substr(1, fieldName.length() - 2); // Remove quotes
                sort(fieldName); // Execute sort
            }
            else{
                throw WrongCommandException(); // Throw for unknown commands
            }
        }catch(const WrongCommandException&){
            logException("command is wrong"); // Log wrong command error
        }
    }
}

// Search for entries matching value in specified field
void Catalog::search(const string& value, const string& fieldName){
    logCommand("search " + fieldName + " " + value); // Log search command
    
    int fieldIndex = findFieldIndex(fieldName); // Find field index
    if(fieldIndex == -1){
        logException("Field not found: " + fieldName); // Log if field not found
        return;
    }

    bool found = false; // Track if any matches found
    for(const Entry& entry : entries){
        if(entry.match(value, fieldName, fieldFormats)){ // Check if entry matches
            logOutput(entry.toString()); // Log matching entry
            found = true; // Mark as found
        }
    }

    if(!found){
        logOutput("No entries found"); // Log if no matches
    }
}

// Bubble sort entries by specified field using string comparison
void Catalog::bubbleSort(size_t fieldIndex){
    bool swapped; // Flag to track if any swaps occurred
    for(size_t i = 0; i < entries.size() - 1; i++){
        swapped = false;
        for(size_t j = 0; j < entries.size() - i - 1; j++){
            const FieldBase* field1 = entries[j].getField(fieldIndex);
            const FieldBase* field2 = entries[j + 1].getField(fieldIndex);

            if(field1 && field2 && field1->toString() > field2->toString()){ // Compare field values
                Entry temp = entries[j]; // Swap entries
                entries[j] = entries[j + 1];
                entries[j + 1] = temp;
                swapped = true; // Mark that swap occurred
            }
        }

        if(!swapped){
            break; // Exit early if no swaps in this pass
        }
    }
}

// Sort entries by specified field and log results
void Catalog::sort(const string& fieldName){
    int fieldIndex = findFieldIndex(fieldName); // Find field index
    if(fieldIndex == -1){
        throw WrongCommandException(); // Throw if field not found
    }
    bubbleSort(static_cast<size_t>(fieldIndex)); // Sort entries

    for(size_t i = 0; i < entries.size(); i++){
        logOutput(entries[i].toString()); // Log each sorted entry
    }
}