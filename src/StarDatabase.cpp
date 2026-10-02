#include "super_luminal_pch.h"
#include "StarDatabase.h"
#include "GaiaInterface.h"
#include <cmath>
#include <iostream>
#include <algorithm>

StarDatabase::StarDatabase() {}

StarDatabase::~StarDatabase() {
    if (m_loadThread.joinable()) {
        m_loadThread.join();
    }
}

void StarDatabase::loadFromGaiaAPIAsync(int limit) {
    if (m_isLoading) return;
    
    m_isLoading = true;
    m_loadThread = std::thread([this, limit]() {
        std::vector<Star> fetched_stars = GaiaInterface::fetchTopStars(limit);
        
        std::lock_guard<std::mutex> lock(m_mutex);
        m_all_stars = fetched_stars;
        m_named_stars.clear();
        
        for (const auto& star : m_all_stars) {
            if (!star.proper_name.empty()) {
                m_named_stars.push_back(star);
            }
        }
        
        // Sort named stars by distance from Sol
        std::sort(m_named_stars.begin(), m_named_stars.end(), [](const Star& a, const Star& b) {
            return a.dist < b.dist;
        });
        
        m_isLoading = false;
        std::cout << "Loaded " << m_all_stars.size() << " stars via Gaia API." << std::endl;
    });
}

std::vector<Star> StarDatabase::getNamedStars() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_named_stars;
}

float StarDatabase::calculateDistanceLY(const Star& a, const Star& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    float dist_parsecs = std::sqrt(dx*dx + dy*dy + dz*dz);
    return dist_parsecs * 3.26156f; // 1 parsec = 3.26156 light-years
}
