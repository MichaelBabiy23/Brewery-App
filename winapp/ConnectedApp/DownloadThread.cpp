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
    auto res = cli.Get("/breweries");
    if (res && res->status == 200) {
        auto json_result = nlohmann::json::parse(res->body);
        std::cout << json_result.dump(4) << '\n';
        
        for (const auto& brewery : json_result) {
            Brewery b;
            b.id = brewery.contains("id") && !brewery["id"].is_null() ? brewery["id"].get<std::string>() : "";
            b.name = brewery.contains("name") && !brewery["name"].is_null() ? brewery["name"].get<std::string>() : "";
            b.brewery_type = brewery.contains("brewery_type") && !brewery["brewery_type"].is_null() ? brewery["brewery_type"].get<std::string>() : "";
            b.street = brewery.contains("street") && !brewery["street"].is_null() ? brewery["street"].get<std::string>() : "";
            b.city = brewery.contains("city") && !brewery["city"].is_null() ? brewery["city"].get<std::string>() : "";
            b.state = brewery.contains("state") && !brewery["state"].is_null() ? brewery["state"].get<std::string>() : "";
            b.postal_code = brewery.contains("postal_code") && !brewery["postal_code"].is_null() ? brewery["postal_code"].get<std::string>() : "";
            b.country = brewery.contains("country") && !brewery["country"].is_null() ? brewery["country"].get<std::string>() : "";
            b.phone = brewery.contains("phone") && !brewery["phone"].is_null() ? brewery["phone"].get<std::string>() : "";
            b.website_url = brewery.contains("website_url") && !brewery["website_url"].is_null() ? brewery["website_url"].get<std::string>() : "";


            common.breweries.push_back(b);
        }
        common.data_ready = true;
    }
    else {
        std::cerr << "Failed to download data.\n";
    }
}

void DownloadThread::SetUrl(const std::string& new_url) {
    _download_url = new_url;
}
