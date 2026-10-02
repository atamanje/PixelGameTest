#pragma once
#include <vector>
#include <string>
#include "StarDatabase.h"

struct SimulationData {
    // Inputs
    float distance_ly = 4.24f; // Default: Proxima Centauri
    float velocity_c = 0.5f;   // Default: 0.5c

    // Outputs
    float time_external_yr = 0.0f;
    float time_ship_yr = 0.0f;

    // Warp Field Plot Data
    std::vector<float> warp_field_x;
    std::vector<float> warp_field_theta;

    // Alcubierre Metric Constants
    float warp_sigma = 8.0f; // Bubble wall thickness parameter
    float warp_R = 1.0f;     // Bubble radius

    // Journey Map Data
    float trip_progress = 0.0f; // 0.0 to 1.0
    bool is_animating = false;
    float animation_speed = 0.1f; // Progress per second
    
    StarDatabase star_db;
    Star start_star;
    Star dest_star;

    SimulationData() {
        // Initialize plot vectors
        int points = 500;
        warp_field_x.resize(points, 0.0f);
        warp_field_theta.resize(points, 0.0f);
    }
};
