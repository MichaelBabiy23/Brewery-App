#pragma once
#include <atomic>
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
    std::atomic_bool exit_flag = false;
    std::atomic_bool start_download = false;
    std::atomic_bool data_ready = false;
    std::string url;
    std::vector<Brewery> breweries;
    Brewery* current_brewery = nullptr; // Pointer to the currently selected brewery
};
