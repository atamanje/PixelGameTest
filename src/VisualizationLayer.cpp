#include "super_luminal_pch.h"
#include "VisualizationLayer.h"

VisualizationLayer::VisualizationLayer(SimulationData* data) : m_data(data) {}

void VisualizationLayer::render() {
    ImGui::Begin("Plotting Window"); // Maps to the right panel
    
    if (ImGui::BeginTabBar("VisualizationTabs")) {
        
        if (ImGui::BeginTabItem("Journey Map")) {
            if (ImPlot::BeginPlot("2D Journey Map", ImVec2(-1, -1), ImPlotFlags_Equal)) {
                // Get all named stars and plot them as background
                const auto& stars = m_data->star_db.getNamedStars();
                std::vector<double> bg_x, bg_y;
                for (const auto& star : stars) {
                    bg_x.push_back(star.x * 3.26156f);
                    bg_y.push_back(star.y * 3.26156f);
                }

                // Plot Start and End Stars
                double start_x = m_data->start_star.x * 3.26156f;
                double start_y = m_data->start_star.y * 3.26156f;
                double dest_x = m_data->dest_star.x * 3.26156f;
                double dest_y = m_data->dest_star.y * 3.26156f;

                // Configure axes to show start and end with some padding
                float pad = m_data->distance_ly * 0.2f;
                if (pad < 5.0f) pad = 5.0f;
                float min_x = (float)std::min(start_x, dest_x) - pad;
                float max_x = (float)std::max(start_x, dest_x) + pad;
                float min_y = (float)std::min(start_y, dest_y) - pad;
                float max_y = (float)std::max(start_y, dest_y) + pad;
                
                ImPlot::SetupAxes("X (Ly)", "Y (Ly)", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
                ImPlot::SetupAxesLimits(min_x, max_x, min_y, max_y, ImGuiCond_Always);

                ImPlotSpec spec_nearby;
                spec_nearby.Marker = ImPlotMarker_Circle;
                spec_nearby.MarkerSize = 2.0f;
                spec_nearby.MarkerFillColor = ImVec4(0.5f, 0.5f, 0.5f, 0.5f);
                if (!bg_x.empty()) {
                    ImPlot::PlotScatter("Nearby Stars", bg_x.data(), bg_y.data(), (int)bg_x.size(), spec_nearby);
                }

                ImPlotSpec spec_start;
                spec_start.Marker = ImPlotMarker_Circle;
                spec_start.MarkerSize = 5.0f;
                spec_start.MarkerFillColor = ImVec4(0.5f, 0.8f, 1.0f, 1.0f);
                ImPlot::PlotScatter(m_data->start_star.proper_name.c_str(), &start_x, &start_y, 1, spec_start);
                
                ImPlotSpec spec_dest;
                spec_dest.Marker = ImPlotMarker_Circle;
                spec_dest.MarkerSize = 5.0f;
                spec_dest.MarkerFillColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f);
                ImPlot::PlotScatter(m_data->dest_star.proper_name.c_str(), &dest_x, &dest_y, 1, spec_dest);

                // Draw connecting line
                double line_x[2] = { start_x, dest_x };
                double line_y[2] = { start_y, dest_y };
                ImPlotSpec spec_line;
                spec_line.LineColor = ImVec4(1,1,1,0.3f);
                ImPlot::PlotLine("Journey Path", line_x, line_y, 2, spec_line);

                // Plot Spaceship
                double ship_x = start_x + (dest_x - start_x) * m_data->trip_progress;
                double ship_y = start_y + (dest_y - start_y) * m_data->trip_progress;
                
                ImPlotSpec spec_ship;
                spec_ship.Marker = ImPlotMarker_Square;
                spec_ship.MarkerSize = IMPLOT_AUTO;
                spec_ship.MarkerFillColor = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
                ImPlot::PlotScatter("Spaceship", &ship_x, &ship_y, 1, spec_ship);

                // Annotations
                ImPlot::Annotation(ship_x, ship_y, ImVec4(1.0f,1.0f,1.0f,1.0f), ImVec2(0, -10), true, "Ship");
                ImPlot::Annotation(start_x, start_y, ImVec4(0.5f,0.8f,1.0f,1.0f), ImVec2(0, 10), true, m_data->start_star.proper_name.c_str());
                ImPlot::Annotation(dest_x, dest_y, ImVec4(1.0f,0.5f,0.5f,1.0f), ImVec2(0, 10), true, m_data->dest_star.proper_name.c_str());
                
                ImPlot::EndPlot();
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Warp Field")) {
            if (m_data->velocity_c > 1.0f) {
                ImGui::Text("Warp Field Visualization (2D Cross-section)");
                ImGui::TextDisabled("(Designed for 3D upgrade in future)");
                if (ImPlot::BeginPlot("Alcubierre Warp Field (Expansion Theta)", ImVec2(-1, -1))) {
                    ImPlot::SetupAxes("Position (x)", "Expansion Scalar (Theta)", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
                    ImPlot::PlotLine("Warp Bubble", m_data->warp_field_x.data(), m_data->warp_field_theta.data(), (int)m_data->warp_field_x.size());
                    
                    // Draw a marker for the ship
                    double ship_warp_x = 0;
                    double ship_warp_y = 0;
                    ImPlot::PlotScatter("Ship", &ship_warp_x, &ship_warp_y, 1);
                    
                    ImPlot::EndPlot();
                }
            } else {
                ImGui::Text("Sublight Travel. No warp field active.");
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    
    ImGui::End();
}
