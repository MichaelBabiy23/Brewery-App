// Enable OpenSSL support for HTTP client
#define CPPHTTPLIB_OPENSSL_SUPPORT
// Include the header for the DownloadThread class
#include "DownloadThread.h"
// Include the HTTP client library
#include "httplib.h"
// Include the JSON library for parsing and serializing JSON
#include "nlohmann/json.hpp"
// Include atomic for atomic operations
#include <atomic>
// Include string for string manipulations
#include <string>
// Include vector for dynamic array
#include <vector>
// Include iostream for input and output stream
#include <iostream>
// Include thread for multithreading
#include <thread>

// Define the serialization/deserialization for the Brewery struct
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Brewery, id, name, brewery_type, street, city, state, postal_code, country, phone, website_url)

// This function is the thread's main function, executing the download logic
void DownloadThread::operator()(CommonObjects& common) {
    // Check if the download URL is set
    if (_download_url.empty()) {
        std::cerr << "Download URL not set.\n";
        return;
    }

    // Initialize the HTTP client with the download URL
    httplib::Client cli(_download_url.c_str());

    while (true) {
        std::string country, search, type;
        // Retrieve the current parameters in a thread-safe manner
        {
            std::lock_guard<std::mutex> lock_guard(common.mutex);
            country = common.current_countries;
            search = common.current_search;
            type = common.current_type;
            // Reset the common object's current parameters
            common.current_countries = "";
            common.current_type = "";
            common.current_search = "";
            common.reset = false;
        }

        // Generate the request URL
        std::string url = GenerateUrl(country, search, type);
        if (!url.empty()) {
            // Send the GET request
            auto response = cli.Get(url);
            if (response && response->status == 200) {
                // Parse the JSON response
                auto json_result = nlohmann::json::parse(response->body);
                // Parse and store the breweries data
                ParseAndStoreBreweries(response->body, common);
                common.data_ready = true;
            }
            else {
                std::cerr << "Failed to download data.\n";
            }
        }

        // Wait for further instructions
        std::unique_lock<std::mutex> lock(common.mutex);
        common.cv.wait(lock, [&] { return common.exit_flag || common.current_type != "" || common.current_countries != "" || common.current_search != "" || common.reset; });

        // Exit the loop if the exit flag is set
        if (common.exit_flag)
            return;

        common.data_ready = false;
    }
}

// Generate the request URL based on the parameters
std::string DownloadThread::GenerateUrl(const std::string& country, const std::string& search, const std::string& type) {
    std::string url = "/breweries";
    if (!country.empty()) {
        // Append the country parameter to the URL
        return url + "?by_country=" + country + "&per_page=100";
    }
    else if (!search.empty()) {
        // Append the search parameter to the URL
        return url + "/search?query=" + search + "&per_page=100";
    }
    else if (!type.empty()) {
        // Append the type parameter to the URL
        return url + "?by_type=" + type + "&per_page=100";
    }
    return url;
}

// Parse the JSON response and store the breweries data
void DownloadThread::ParseAndStoreBreweries(const std::string& body, CommonObjects& common) {
    auto json_result = nlohmann::json::parse(body);

    // Iterate through each brewery in the JSON array
    for (const auto& brewery : json_result) {
        Brewery b;

        // Extract the brewery fields
        b.id = GetBreweryField(brewery, "id");
        b.name = GetBreweryField(brewery, "name");
        b.brewery_type = GetBreweryField(brewery, "brewery_type");
        b.street = GetBreweryField(brewery, "street");
        b.city = GetBreweryField(brewery, "city");
        b.state = GetBreweryField(brewery, "state");
        b.postal_code = GetBreweryField(brewery, "postal_code");
        b.country = GetBreweryField(brewery, "country");
        b.phone = GetBreweryField(brewery, "phone");
        b.website_url = GetBreweryField(brewery, "website_url");

        // Store the brewery in the common object
        common.breweries.push_back(b);
    }
}

// Safely extract a field from the brewery JSON object
std::string DownloadThread::GetBreweryField(const auto brewery, const std::string& field_name) {
    return brewery.contains(field_name) && !brewery[field_name].is_null()
        ? brewery[field_name].get<std::string>()
        : "";
}

// Set the download URL
void DownloadThread::SetUrl(const std::string& new_url) {
    _download_url = new_url;
}
