//
// Created by Immanuel Bierer on 12/12/2023.
//

#ifndef LIVEWALLPAPER05_SPHEREMAP_H
#define LIVEWALLPAPER05_SPHEREMAP_H


class SphereMap : public EnvironmentMap {
public:

    // Default Constructor
    SphereMap();

    // Destructor
    ~SphereMap();

    // Copy Constructor
    SphereMap(const SphereMap& other);

    // Assignment Operator
    SphereMap& operator=(const SphereMap& other);

    GLuint getTextureId();

private:

    GLuint textureId;

};


#endif //LIVEWALLPAPER05_SPHEREMAP_H
