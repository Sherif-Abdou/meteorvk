//
// Created by Sherif Abdou on 11/20/23.
//

#ifndef MATERIAL_H
#define MATERIAL_H
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


/// Material object used to shade objects, stores data such as albedo
class Material {
private:
    glm::vec4 albedo {};

public:
    glm::vec3 getColor();
    void setColor(glm::vec3 color);

    float getSpecular();
    void setSpecular(float specular);
};



#endif //MATERIAL_H
