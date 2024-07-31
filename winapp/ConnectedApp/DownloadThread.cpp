#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "DownloadThread.h"
#include "httplib.h"
#include "nlohmann/json.hpp"
#include <atomic>
#include <string>
#include <vector>
#include <iostream>
#include <thread>

// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE macro can be used to simplify JSON serialization/deserialization
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Brewery, id, name, brewery_type, street, city, state, postal_code, country, phone, website_url)

void DownloadThread::operator()(CommonObjects& common) {
    if (_download_url.empty()) {
        std::cerr << "Download URL not set.\n";
        return;
    }

    httplib::Client cli(_download_url.c_str());


    while (true) {
        std::string country, search, type;
        // Retrieve the current parameters in a thread-safe manner
        {
            std::lock_guard<std::mutex> lock_gurd(common.mutex);
            country = common.current_countries;
            search = common.current_serach;
            type = common.current_type;
            common.current_countries = "";
            common.current_type = "";
            common.current_serach = "";
            common.reset = false;
        }

        // Generate the request URL
        std::string url = GenerateUrl(country, search, type);
        if (!url.empty()) {

            auto response = cli.Get(url);
            if (response && response->status == 200) {
                auto json_result = nlohmann::json::parse(response->body);
                //std::cout << json_result.dump(4) << '\n';
                ParseAndStoreBreweries(response->body, common);
                common.data_ready = true;
            }
            else {
                std::cerr << "Failed to download data.\n";
            }
        }

        // Wait for further instructions
        std::unique_lock<std::mutex> lock(common.mutex);
        common.cv.wait(lock, [&] { return common.exit_flag || common.current_type != "" || common.current_countries != "" || common.current_serach != "" || common.reset; });

        if (common.exit_flag)
            return;

        common.data_ready = false;
    }
}

std::string DownloadThread::GenerateUrl(const std::string& country, const std::string& search, const std::string& type) {
    std::string url = "/breweries";
    if (!country.empty()) {
        return url + "?by_country=" + country + "&per_page=100";
    }
    else if (!search.empty()) {
        return url + "/search?query=" + search + "&per_page=100";
    }
    else if (!type.empty()) {
        return url + "?by_type=" + type + "&per_page=100";
    }
    return url;
}

void DownloadThread::ParseAndStoreBreweries(const std::string& body, CommonObjects& common) {
    auto json_result = nlohmann::json::parse(body);
    //std::cout << json_result.dump(4) << '\n';

    for (const auto& brewery : json_result) {
        Brewery b;

        // Use a helper function to extract the values safely
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

        common.breweries.push_back(b);
    }
}

std::string DownloadThread::GetBreweryField(const auto brewery, const std::string& field_name) {
    return brewery.contains(field_name) && !brewery[field_name].is_null()
        ? brewery[field_name].get<std::string>()
        : "";
}

void DownloadThread::SetUrl(const std::string& new_url) {
    _download_url = new_url;
}
