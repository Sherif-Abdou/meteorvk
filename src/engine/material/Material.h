//
// Created by Sherif Abdou on 11/20/23.
//

#ifndef MATERIAL_H
#define MATERIAL_H
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct AtlasRect {
    float x1;
    float x2;
    float y1;
    float y2;

    static AtlasRect createDefault() {
        return {0.0, 1.0, 0.0, 1.0};
    }
};

/// Material object used to shade objects, stores data such as albedo
class Material {
public:
    explicit Material(const glm::vec4& albedo)
        : albedo(albedo) {
    }
private:
    glm::vec4 albedo {};
    AtlasRect atlas = AtlasRect::createDefault();

public:
    glm::vec3 getColor();
    void setColor(glm::vec3 color);

    float getSpecular();
    void setSpecular(float specular);
};



#endif //MATERIAL_H
