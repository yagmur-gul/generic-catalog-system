#include "catalog.h"

int main(){
    try{
        // Create catalog object with output log file
        Catalog catalog("output.txt");
        
        // Read and parse data from input file
        catalog.readData("data.txt");
        
        // Process search and sort commands
        catalog.processCommands("commands.txt");
        
        return 0; // Success
    }catch(const std::exception& e){
        // Handle any exceptions that occur during execution
        std::cerr << "Error: " << e.what() << std::endl;
        return 1; // Error exit code
    }
}