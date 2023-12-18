//
// Created by Immanuel Bierer on 12/12/2023.
//

#ifndef LIVEWALLPAPER05_SPHEREMAP_H
#define LIVEWALLPAPER05_SPHEREMAP_H


class SphereMap : public EnvironmentMap {
public:

    SphereMap();

    SphereMap(Texture::DefaultImages option, const int& w, const int& h);

    // Destructor
    ~SphereMap();

    // Copy Constructor
    SphereMap(const SphereMap& other);

    // Assignment Operator
    SphereMap& operator=(const SphereMap& other);

private:

};


#endif //LIVEWALLPAPER05_SPHEREMAP_H