#pragma once
#include <string>
#include <vector>
#include <atomic>
#include <mutex>
#include <thread>

struct Star {
    int id;
    std::string proper_name;
    float dist; // in parsecs
    float x;    // in parsecs
    float y;    // in parsecs
    float z;    // in parsecs
};

class StarDatabase {
public:
    StarDatabase();
    ~StarDatabase();

    void loadFromGaiaAPIAsync(int limit = 1000);

    bool isLoading() const { return m_isLoading; }

    std::vector<Star> getNamedStars();
    
    // Get distance in light-years between two stars
    static float calculateDistanceLY(const Star& a, const Star& b);

private:
    std::vector<Star> m_all_stars;
    std::vector<Star> m_named_stars;
    
    std::atomic<bool> m_isLoading{false};
    std::mutex m_mutex;
    std::thread m_loadThread;
};
