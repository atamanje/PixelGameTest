#include "super_luminal_pch.h"
#include "GaiaInterface.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <cmath>

using json = nlohmann::json;

size_t GaiaInterface::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

std::vector<Star> GaiaInterface::fetchTopStars(int limit) {
    std::vector<Star> stars;
    
    // Add Sol manually since Gaia doesn't include it
    Star sol;
    sol.id = 0;
    sol.proper_name = "Sol";
    sol.dist = 0.0f;
    sol.x = 0.0f;
    sol.y = 0.0f;
    sol.z = 0.0f;
    stars.push_back(sol);

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize libcurl" << std::endl;
        return stars;
    }

    std::string response_string;
    std::string url = "https://gea.esac.esa.int/tap-server/tap/sync";
    
    // ADQL Query: Top closest stars (largest parallax)
    std::string query = "REQUEST=doQuery&LANG=ADQL&FORMAT=json&QUERY=";
    std::string adql = "SELECT TOP " + std::to_string(limit) + " designation, ra, dec, parallax, phot_g_mean_mag FROM gaiadr3.gaia_source WHERE parallax IS NOT NULL ORDER BY parallax DESC";
    
    // URL-encode the ADQL query
    char* encoded_adql = curl_easy_escape(curl, adql.c_str(), (int)adql.length());
    if (encoded_adql) {
        query += encoded_adql;
        curl_free(encoded_adql);
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, query.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    // Add User-Agent as some APIs require it
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
        return stars;
    }
    
    curl_easy_cleanup(curl);

    try {
        auto j = json::parse(response_string);
        if (j.contains("data") && j["data"].is_array()) {
            int id_counter = 1;
            for (const auto& row : j["data"]) {
                // row is an array: [designation, ra, dec, parallax, phot_g_mean_mag]
                if (row.size() < 4) continue;
                if (row[1].is_null() || row[2].is_null() || row[3].is_null()) continue;
                
                Star s;
                s.id = id_counter++;
                
                // TODO: mapping Gaia designations to common names
                s.proper_name = row[0].is_string() ? row[0].get<std::string>() : "Unknown";
                
                float ra_deg = row[1].get<float>();
                float dec_deg = row[2].get<float>();
                float parallax_mas = row[3].get<float>();
                
                if (parallax_mas <= 0) continue; // invalid or infinitely far
                
                s.dist = 1000.0f / parallax_mas; // distance in parsecs
                
                // Convert degrees to radians
                float ra_rad = ra_deg * (3.14159265f / 180.0f);
                float dec_rad = dec_deg * (3.14159265f / 180.0f);
                
                s.x = s.dist * std::cos(dec_rad) * std::cos(ra_rad);
                s.y = s.dist * std::cos(dec_rad) * std::sin(ra_rad);
                s.z = s.dist * std::sin(dec_rad);
                
                stars.push_back(s);
            }
        } else if (j.contains("ERRORS")) {
            std::cerr << "Gaia API Error: " << j["ERRORS"] << std::endl;
        }
    } catch (const json::parse_error& e) {
        std::cerr << "JSON Parse error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception processing Gaia data: " << e.what() << std::endl;
    }

    return stars;
}
