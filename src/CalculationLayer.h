#pragma once
#include "SimulationData.h"

class CalculationLayer {
public:
    CalculationLayer(SimulationData* data);
    void render();

private:
    SimulationData* m_data;
};
