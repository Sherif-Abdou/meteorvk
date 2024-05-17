#version 450
#extension GL_EXT_nonuniform_qualifier : enable

#ifndef UBO_BINDING
#define UBO_BINDING 0
#endif

layout(location = 0) out vec4 color;
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 light_space_position;
layout(location = 5) flat in uint material_id;

const float PI = 3.14159265359;

layout(binding = UBO_BINDING) uniform UBO {
    mat4 proj;
    mat4 view;
};

void main() {
    color = vec4(0.8, 0.2, 0.0, 1.0);
}