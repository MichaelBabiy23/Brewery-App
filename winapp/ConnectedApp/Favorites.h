#pragma once
#ifndef FAVORITES_H
#define FAVORITES_H

#include <string>
#include <vector>
#include "CommonObject.h"

bool isFavorite(const std::string& id);
void toggleFavorite(const Brewery& brewery);
std::vector<Brewery> getFavorites();

#endif