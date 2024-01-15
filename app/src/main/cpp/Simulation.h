//
// Created by Immanuel Bierer on 1/6/2024.
//

#ifndef LIVEWALLPAPER05_SIMULATION_H
#define LIVEWALLPAPER05_SIMULATION_H


class Simulation : public Computation {

public:

    struct Particle {
        vec3 position;
        vec3 velocity;
    };

    int particleCount;

    int cbrtParticleCount;

    void setParticleCount(int n);

private:

};


#endif //LIVEWALLPAPER05_SIMULATION_H
