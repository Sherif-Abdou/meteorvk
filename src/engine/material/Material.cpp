//
// Created by Sherif Abdou on 11/20/23.
//

#include <cstdint>
#include "Material.h"

glm::vec3 Material::getColor() {
    return glm::vec3(albedo);
}

void Material::setColor(glm::vec3 color) {
    albedo = glm::vec4(color, albedo.a);
}

float Material::getSpecular() {
    return albedo.a;
}

void Material::setSpecular(float specular) {
    albedo.a = specular;
}

const int32_t &Material::getTextureId() const {
    return textureID;
}

void Material::setTextureId(const int32_t &textureId) {
    textureID = textureId;
}

const int32_t &Material::getSamplerId() const {
    return samplerID;
}

void Material::setSamplerId(const int32_t &samplerId) {
    samplerID = samplerId;
}
