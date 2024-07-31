#pragma once
#include "CommonObject.h"

class DownloadThread
{
public:
	void operator()(CommonObjects& common);
	void SetUrl(const std::string& new_url);
private:
	std::string _download_url;
	std::string GenerateUrl(const std::string& country, const std::string& search, const std::string& type);
	void ParseAndStoreBreweries(const std::string& body, CommonObjects& common);
	std::string GetBreweryField(const auto brewery, const std::string& field_name);
};

