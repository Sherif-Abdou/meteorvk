#version 450
#extension GL_EXT_nonuniform_qualifier : enable

// #ifndef GLOBAL_UBO_BINDING
// #define GLOBAL_UBO_BINDING 0
// #endif

layout(location = 0) out vec4 color;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 light_space_position;
layout(location = 5) flat in uint material_id;

const float PI = 3.14159265359;

layout(set = GLOBAL_UBO_SET, binding = GLOBAL_UBO_BINDING) uniform UBO {
    mat4 proj;
    mat4 view;
};

layout(set = OCCLUSION_MAP_SET, binding = OCCLUSION_MAP_BINDING) uniform sampler2D occlusion_map;

float calculateOcclusion() {
    vec2 multiplier = 1.0f / vec2(2560.0f, 1440.0f);
    vec2 screen_uv = gl_FragCoord.xy * multiplier;

    float occlusion = texture(occlusion_map, screen_uv).r;
    float sum = 0.0;
    int c = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            float value = texture(occlusion_map, screen_uv + vec2(i, j) * multiplier).r;
            sum += value;
            c++;
        }
    }

    return sum / (float(c));
}

void main() {
    float occlusion = calculateOcclusion();
    color = vec4(0.8 * occlusion, 0.2 * occlusion, 0.0, 1.0);
}
