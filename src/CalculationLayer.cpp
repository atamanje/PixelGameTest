#include "super_luminal_pch.h"
#include "CalculationLayer.h"
#include "PhysicsCalculations.h"

CalculationLayer::CalculationLayer(SimulationData* data) : m_data(data) {}

void CalculationLayer::render() {
    // Handle Animation
    if (m_data->is_animating) {
        m_data->trip_progress += (ImGui::GetIO().DeltaTime * m_data->animation_speed);
        if (m_data->trip_progress >= 1.0f) {
            m_data->trip_progress = 0.0f; // Loop back
        }
    }

    // Perform calculations every frame based on inputs
    Physics::CalculateTripTimes(m_data->distance_ly, m_data->velocity_c, m_data->time_external_yr, m_data->time_ship_yr);

    // Calculate warp field data for plot
    // We'll plot from x = -3.0 to x = 3.0 relative to ship center
    float x_min = -3.0f;
    float x_max = 3.0f;
    int points = (int)m_data->warp_field_x.size();
    float step = (x_max - x_min) / (float)(points - 1);

    for (int i = 0; i < points; ++i) {
        float x = x_min + i * step;
        m_data->warp_field_x[i] = x;
        
        if (m_data->velocity_c > 1.0f) {
            // Plot Alcubierre expansion if > 1c
            m_data->warp_field_theta[i] = Physics::AlcubierreExpansionTheta(x, m_data->velocity_c, m_data->warp_R, m_data->warp_sigma);
        } else {
            // Flat spacetime if sublight
            m_data->warp_field_theta[i] = 0.0f;
        }
    }

    // Display Window for Calculated Data
    ImGui::Begin("Calculation Window");
    ImGui::Text("Trip Information");
    ImGui::Separator();
    
    ImGui::Text("Distance: %.2f Light-years", m_data->distance_ly);
    ImGui::Text("Velocity: %.4f c", m_data->velocity_c);
    
    ImGui::Spacing();
    float ext_days = m_data->time_external_yr * 365.25f;
    float ship_days = m_data->time_ship_yr * 365.25f;
    ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "External Observer Time: %.3f Days (%.3f Years)", ext_days, m_data->time_external_yr);
    ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Ship Experienced Time: %.3f Days (%.3f Years)", ship_days, m_data->time_ship_yr);

    if (m_data->velocity_c <= 1.0f) {
        float time_dilation_factor = m_data->time_external_yr > 0 ? (m_data->time_external_yr / m_data->time_ship_yr) : 1.0f;
        ImGui::Text("Time Dilation Factor (Gamma): %.2f", time_dilation_factor);
    } else {
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Status: Warp Drive Active");
    }

    ImGui::End();
}
