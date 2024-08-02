#include "Favorites.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

const std::string FAVORITES_FILE = "favorites.json";

bool isFavorite(const std::string& id) {
    std::ifstream file(FAVORITES_FILE);
    if (!file.is_open()) {
        return false;
    }

    nlohmann::json favorites_json;
    file >> favorites_json;
    file.close();

    for (const auto& item : favorites_json) {
        if (item["id"] == id) {
            return true;
        }
    }

    return false;
}

void toggleFavorite(const Brewery& brewery) {
    std::ifstream file(FAVORITES_FILE);
    nlohmann::json favorites_json;

    if (file.is_open()) {
        file >> favorites_json;
        file.close();
    }

    bool found = false;
    for (auto it = favorites_json.begin(); it != favorites_json.end(); ++it) {
        if ((*it)["id"] == brewery.id) {
            favorites_json.erase(it);
            found = true;
            break;
        }
    }

    if (!found) {
        favorites_json.push_back({
            {"id", brewery.id},
            {"name", brewery.name},
            {"brewery_type", brewery.brewery_type},
            {"street", brewery.street},
            {"city", brewery.city},
            {"state", brewery.state},
            {"postal_code", brewery.postal_code},
            {"country", brewery.country},
            {"phone", brewery.phone},
            {"website_url", brewery.website_url}
            });
    }

    std::ofstream outFile(FAVORITES_FILE);
    outFile << favorites_json.dump(4);
    outFile.close();
}

std::vector<Brewery> getFavorites() {
    std::ifstream file(FAVORITES_FILE);
    std::vector<Brewery> favorites;
    if (!file.is_open()) {
        return favorites;
    }

    nlohmann::json favorites_json;
    file >> favorites_json;
    file.close();

    for (const auto& item : favorites_json) {
        Brewery brewery;
        brewery.id = item["id"];
        brewery.name = item["name"];
        brewery.brewery_type = item["brewery_type"];
        brewery.street = item["street"];
        brewery.city = item["city"];
        brewery.state = item["state"];
        brewery.postal_code = item["postal_code"];
        brewery.country = item["country"];
        brewery.phone = item["phone"];
        brewery.website_url = item["website_url"];
        favorites.push_back(brewery);
    }

    return favorites;
}
