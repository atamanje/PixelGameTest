#pragma once

namespace Physics {
    void CalculateTripTimes(float distance_ly, float velocity_c, float& out_external_yr, float& out_ship_yr);
    float AlcubierreShapeFunction(float r, float R, float sigma);
    float AlcubierreExpansionTheta(float x, float v_s, float R, float sigma);
}
