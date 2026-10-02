#include "super_luminal_pch.h"
#include "PhysicsCalculations.h"
#include <cmath>

namespace Physics {

    void CalculateTripTimes(float distance_ly, float velocity_c, float& out_external_yr, float& out_ship_yr) {
        if (velocity_c <= 0.0f) {
            out_external_yr = 0.0f;
            out_ship_yr = 0.0f;
            return;
        }

        // External time t = d / v
        out_external_yr = distance_ly / velocity_c;

        if (velocity_c <= 1.0f) {
            // Sublight: Apply time dilation (Lorentz factor)
            // t_ship = t_external * sqrt(1 - v^2/c^2)
            float lorentz_inv = std::sqrt(1.0f - velocity_c * velocity_c);
            out_ship_yr = out_external_yr * lorentz_inv;
        } else {
            // FTL (Warp): Ship is in flat spacetime bubble
            out_ship_yr = out_external_yr;
        }
    }

    float AlcubierreShapeFunction(float r, float R, float sigma) {
        float tanh_plus = std::tanh(sigma * (r + R));
        float tanh_minus = std::tanh(sigma * (r - R));
        float tanh_denom = 2.0f * std::tanh(sigma * R);
        return (tanh_plus - tanh_minus) / tanh_denom;
    }

    float AlcubierreExpansionTheta(float x, float v_s, float R, float sigma) {
        float r_s = std::abs(x);
        if (r_s == 0.0f) return 0.0f;

        float cosh_plus = std::cosh(sigma * (r_s + R));
        float cosh_minus = std::cosh(sigma * (r_s - R));
        float sech2_plus = 1.0f / (cosh_plus * cosh_plus);
        float sech2_minus = 1.0f / (cosh_minus * cosh_minus);
        
        float df_dr = (sigma / (2.0f * std::tanh(sigma * R))) * (sech2_plus - sech2_minus);
        
        float theta = v_s * (x / r_s) * df_dr;
        return theta;
    }

}
