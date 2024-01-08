//
// Created by Immanuel Bierer on 1/6/2024.
//

#ifndef LIVEWALLPAPER05_COMPUTATION_H
#define LIVEWALLPAPER05_COMPUTATION_H


class Computation {

public:

    enum ComputationOptions {
        CPU,
        GPU
    };

    ComputeShader computeShader;

protected:

    ComputationOptions computationOption;

    bool computeShaderGenerated = false;

private:

};


#endif //LIVEWALLPAPER05_COMPUTATION_H
