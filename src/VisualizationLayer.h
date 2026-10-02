#pragma once
#include "SimulationData.h"

class VisualizationLayer {
public:
    VisualizationLayer(SimulationData* data);
    void render();

private:
    SimulationData* m_data;
};
