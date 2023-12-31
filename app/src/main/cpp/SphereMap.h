//
// Created by Immanuel Bierer on 12/12/2023.
//

#ifndef LIVEWALLPAPER05_SPHEREMAP_H
#define LIVEWALLPAPER05_SPHEREMAP_H


class SphereMap : public EnvironmentMap {
public:

    const string directionToSphereMapUV =
            "vec2 SPHERE_MAP_TEXTURE_FUNCTION(vec3 direction) {\n"
            "    // Normalize the direction vector\n"
            "    vec3 normalizedDirection = normalize(direction);\n"
            "    // Calculate polar angle (theta) and azimuthal angle (phi)\n"
            "    float theta = acos(normalizedDirection.z); // polar angle\n"
            "    float phi = atan(normalizedDirection.y, normalizedDirection.x); // azimuthal angle\n"
            "    // Map angles to UV coordinates\n"
            "    float u = phi / (2.0 * 3.14159265359) + 0.5;\n"
            "    float p = 1.0 - theta / 3.14159265359;\n"
            "    return vec2(u, p);\n"
            "}\n";

    SphereMap();

    SphereMap(Texture &texture);

    SphereMap(Texture::DefaultImages option, const int &w, const int &h, View *view);

    // Destructor
    ~SphereMap();

    // Copy Constructor
    SphereMap(const SphereMap& other);

    // Assignment Operator
    SphereMap& operator=(const SphereMap& other);

private:

};


#endif //LIVEWALLPAPER05_SPHEREMAP_H