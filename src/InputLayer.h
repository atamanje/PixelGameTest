#pragma once
#include "SimulationData.h"

class InputLayer {
public:
    InputLayer(SimulationData* data);
    void render();

private:
    SimulationData* m_data;
};
