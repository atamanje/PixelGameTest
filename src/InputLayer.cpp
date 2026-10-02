#include "super_luminal_pch.h"
#include "InputLayer.h"
#include <algorithm>
#include <cmath>

InputLayer::InputLayer(SimulationData* data) : m_data(data) {}

void InputLayer::render() {
    ImGui::Begin("Data Window"); // Maps to the left panel
    ImGui::Text("Simulation Controls");
    ImGui::Separator();

    ImGui::Text("Destination Distance");
    ImGui::SliderFloat("Light-years", &m_data->distance_ly, 0.1f, 100000.0f, "%.1f Ly", ImGuiSliderFlags_Logarithmic);
    ImGui::SameLine();
    ImGui::PushItemWidth(100);
    ImGui::InputFloat("##LY_input", &m_data->distance_ly, 0.0f, 0.0f, "%.1f");
    ImGui::PopItemWidth();
    
    ImGui::Spacing();
    ImGui::Text("Ship Velocity");
    ImGui::SliderFloat("v/c", &m_data->velocity_c, 0.001f, 10.0f, "%.4f c", ImGuiSliderFlags_Logarithmic);
    ImGui::SameLine();
    ImGui::PushItemWidth(100);
    ImGui::InputFloat("##VC_input", &m_data->velocity_c, 0.0f, 0.0f, "%.4f");
    ImGui::PopItemWidth();

    ImGui::Spacing();
    ImGui::Spacing();
    
    auto warpToVelocity = [](float w) {
        if (w <= 9.0f) return std::pow(w, 10.0f / 3.0f);
        if (w >= 10.0f) return (float)INFINITY;
        
        // Between 9 and 10, the exponent increases to match Okuda's curve
        float x = -std::log10(10.0f - w);
        float exponent = (10.0f / 3.0f) + (1.0f / 6.0f) * std::pow(x, 1.78035f);
        return std::pow(w, exponent);
    };
    
    auto velocityToWarp = [](float v) {
        float v9 = std::pow(9.0f, 10.0f / 3.0f);
        if (v <= v9) return std::pow(v, 3.0f / 10.0f);
        
        // Binary search for w in (9.0, 10.0)
        float low = 9.0f;
        float high = 10.0f;
        float w = 9.5f;
        
        auto eval_v = [](float target_w) {
            float x = -std::log10(10.0f - target_w);
            float exponent = (10.0f / 3.0f) + (1.0f / 6.0f) * std::pow(x, 1.78035f);
            return std::pow(target_w, exponent);
        };
        
        for (int i = 0; i < 40; ++i) {
            w = (low + high) * 0.5f;
            float current_v = eval_v(w);
            if (current_v < v) low = w;
            else high = w;
        }
        return w;
    };

    float warp_factor = velocityToWarp(m_data->velocity_c);
    ImGui::Text("Warp Factor (TNG)");
    if (ImGui::SliderFloat("Warp", &warp_factor, 0.0f, 9.99f, "Warp %.2f")) {
        m_data->velocity_c = warpToVelocity(warp_factor);
    }
    ImGui::SameLine();
    ImGui::PushItemWidth(100);
    if (ImGui::InputFloat("##Warp_input", &warp_factor, 0.0f, 0.0f, "%.2f")) {
        m_data->velocity_c = warpToVelocity(warp_factor);
    }
    ImGui::PopItemWidth();
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Journey Controls");
    
    if (m_data->star_db.isLoading()) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Loading Gaia Data...");
        ImGui::BeginDisabled();
    } else {
        auto stars = m_data->star_db.getNamedStars();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Loaded Stars: %zu", stars.size());
        
        static bool default_set = false;
        if (!default_set && !stars.empty()) {
            for (const auto& star : stars) {
                if (star.proper_name == "Sol") m_data->start_star = star;
                else if (m_data->dest_star.proper_name.empty()) m_data->dest_star = star;
            }
            m_data->distance_ly = StarDatabase::calculateDistanceLY(m_data->start_star, m_data->dest_star);
            default_set = true;
        }
    }
    
    auto icontains = [](const std::string& str, const std::string& substr) {
        if (substr.empty()) return true;
        auto it = std::search(str.begin(), str.end(), substr.begin(), substr.end(),
            [](char ch1, char ch2) { return std::tolower((unsigned char)ch1) == std::tolower((unsigned char)ch2); });
        return it != str.end();
    };

    if (ImGui::BeginCombo("Start Star", m_data->start_star.proper_name.c_str())) {
        static char search_start[64] = "";
        ImGui::PushItemWidth(-1);
        ImGui::InputText("##SearchStart", search_start, 64);
        ImGui::PopItemWidth();
        
        std::string search_str(search_start);
        
        // TODO: Map Gaia DR3 designations to common names (Sirius, Alpha Centauri, etc.)
        for (const auto& star : m_data->star_db.getNamedStars()) {
            if (!icontains(star.proper_name, search_str)) continue;
            
            bool is_selected = (m_data->start_star.id == star.id);
            if (ImGui::Selectable(star.proper_name.c_str(), is_selected)) {
                m_data->start_star = star;
                m_data->distance_ly = StarDatabase::calculateDistanceLY(m_data->start_star, m_data->dest_star);
            }
            if (is_selected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (ImGui::BeginCombo("Destination Star", m_data->dest_star.proper_name.c_str())) {
        static char search_dest[64] = "";
        ImGui::PushItemWidth(-1);
        ImGui::InputText("##SearchDest", search_dest, 64);
        ImGui::PopItemWidth();
        
        std::string search_str(search_dest);
        
        // TODO: Map Gaia DR3 designations to common names (Sirius, Alpha Centauri, etc.)
        for (const auto& star : m_data->star_db.getNamedStars()) {
            if (!icontains(star.proper_name, search_str)) continue;
            
            bool is_selected = (m_data->dest_star.id == star.id);
            if (ImGui::Selectable(star.proper_name.c_str(), is_selected)) {
                m_data->dest_star = star;
                m_data->distance_ly = StarDatabase::calculateDistanceLY(m_data->start_star, m_data->dest_star);
            }
            if (is_selected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (m_data->star_db.isLoading()) {
        ImGui::EndDisabled();
    }
    ImGui::SliderFloat("Trip Progress", &m_data->trip_progress, 0.0f, 1.0f, "%.3f");
    ImGui::SameLine();
    ImGui::PushItemWidth(100);
    ImGui::InputFloat("##Progress_input", &m_data->trip_progress, 0.0f, 0.0f, "%.3f");
    ImGui::PopItemWidth();
    ImGui::Checkbox("Loop Animation", &m_data->is_animating);
    if (m_data->is_animating) {
        ImGui::SliderFloat("Anim Speed", &m_data->animation_speed, 0.01f, 1.0f);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::InputFloat("##Speed_input", &m_data->animation_speed, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Warp Drive Parameters");
    ImGui::SliderFloat("Bubble Radius (R)", &m_data->warp_R, 0.1f, 5.0f);
    ImGui::SameLine();
    ImGui::PushItemWidth(100);
    ImGui::InputFloat("##R_input", &m_data->warp_R, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();

    ImGui::SliderFloat("Wall Thickness (sigma)", &m_data->warp_sigma, 1.0f, 20.0f);
    ImGui::SameLine();
    ImGui::PushItemWidth(100);
    ImGui::InputFloat("##Sigma_input", &m_data->warp_sigma, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();

    ImGui::End();
}
