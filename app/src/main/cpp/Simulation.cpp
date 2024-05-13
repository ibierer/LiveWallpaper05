//
// Created by Immanuel Bierer on 1/6/2024.
//

#include "Simulation.h"

void Simulation::setParticleCount(int n){
    particleCount = n;
    cbrtParticleCount = cbrt(n);
}
