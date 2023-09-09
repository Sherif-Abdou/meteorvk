#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 tangent;

layout(binding = 0) uniform UBO {
    mat4 proj;
    mat4 view;
    mat4 model;
};

void main() {
    gl_Position = proj * view * model * vec4(position, 1.0);
}