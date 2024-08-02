#pragma once
#include <atomic>
#include <mutex>
#include <string>
#include <vector>

struct Brewery
{
    std::string id;
    std::string name;
    std::string brewery_type;
    std::string street;
    std::string city;
    std::string state;
    std::string postal_code;
    std::string country;
    std::string phone;
    std::string website_url;
};

struct CommonObjects
{
    std::atomic_bool exit_flag = false;     // Flag to indicate when the application should exit
    std::atomic_bool start_download = false; // Flag to signal the start of a download operation
    std::atomic_bool data_ready = false;    // Flag to indicate when data is ready to be processed/displayed
    std::condition_variable cv;             // Condition variable for thread synchronization
    std::mutex mutex;                       // Mutex to protect shared resources and ensure thread safety
    std::vector<Brewery> breweries;         // Vector to store brewery data
    Brewery* current_brewery = nullptr;     // Pointer to the currently selected brewery
    std::string current_type = "";          // Current selected type of brewery
    std::string current_countries = "";     // Current selected country or countries
    std::string current_search = "";        // Current search string
    bool reset = false;                     // Flag to indicate if a reset operation has been requested
};

