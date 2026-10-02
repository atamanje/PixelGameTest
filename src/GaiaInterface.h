#pragma once
#include <vector>
#include <string>
#include "StarDatabase.h"

class GaiaInterface {
public:
    // Fetches top stars closest to Sol asynchronously if used in a thread, or synchronously if not
    static std::vector<Star> fetchTopStars(int limit);

private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
};
