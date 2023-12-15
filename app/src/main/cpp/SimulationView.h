//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_SIMULATIONVIEW_H
#define LIVEWALLPAPER05_SIMULATIONVIEW_H


#include "View.h"

class SimulationView : public View {
public:
    int moleculeCount;
    int cbrtMoleculeCount;
    void setMoleculeCount(int n);
    vec3 compensateForOrientation(const vec3& acc);
};


#endif //LIVEWALLPAPER05_SIMULATIONVIEW_H
