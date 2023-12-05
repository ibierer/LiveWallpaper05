//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_SIMULATION_H
#define LIVEWALLPAPER05_SIMULATION_H


class Simulation : public Wallpaper{
public:
    int moleculeCount;
    int cbrtMoleculeCount;
    void setMoleculeCount(int n);
    vec3 compensateForOrientation(const vec3& acc);
};


#endif //LIVEWALLPAPER05_SIMULATION_H
