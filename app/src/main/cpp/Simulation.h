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

    struct Node {
        bool isLeaf;
        Node *parent;
        Node *children[8];
        vec4 centerOfGravity;
        vec3 center;
        float size;
        // Destructor to delete children recursively (not needed due to stack allocation).
        //~Node() {
        //    for (int i = 0; i < 8; ++i) {
        //        if (children[i] != nullptr) {
        //            delete children[i]; // Recursively delete child nodes
        //        }
        //    }
        //}
    };

    Node* nodes = nullptr;

    int nextFreeNode = 0; // Index to keep track of the next free node

    Node* allocateNode();

    struct Star : Particle {
        float mass;
        vec3 initialForce;
        vec3 finalForce;
        vec3 projectedPosition;
        Node* leaf;
    };

    int particleCount;

    int cbrtParticleCount;

    void setParticleCount(int n);

private:

};

#endif //LIVEWALLPAPER05_SIMULATION_H
