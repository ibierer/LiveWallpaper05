//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "SimulationView.h"

void SimulationView::setMoleculeCount(int n){
    moleculeCount = n;
    cbrtMoleculeCount = cbrt(moleculeCount);
}

vec3 SimulationView::compensateForOrientation(const vec3& acc){
    if(width < height) {
        return -1.0f * acc;
    }else{
        if(acc.x < 0.0f) {
            return vec3(-acc.y, acc.x, -acc.z);
        }else{
            return vec3(acc.y, -acc.x, -acc.z);
        }
    }
}