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

struct RenderMaterial {
    vec4 kA;
    vec4 kD;
    vec4 kS;
    float nS;
    uint illum;
    int kD_index;
    int kS_index;
};

layout(set = GLOBAL_UBO_SET, binding = GLOBAL_UBO_BINDING) uniform UBO {
    mat4 proj;
    mat4 view;
    mat4 light_proj_view;
};

layout(set = OCCLUSION_MAP_SET, binding = OCCLUSION_MAP_BINDING) uniform sampler2D occlusion_map;

layout(set = LIGHT_UBO_SET, binding = LIGHT_UBO_BINDING) uniform Lights {
    vec4 positions[64];
    mat4 povs[64];
    int light_count;
};

layout(set = SHADOW_MAP_SET, binding = SHADOW_MAP_BINDING) uniform sampler2D shadow_map;

/*
layout(set = MATERIAL_BUFFER_SET, binding = MATERIAL_BUFFER_BINDING) readonly buffer Materials {
    RenderMaterial materials[64];
};
*/

layout(set = MODEL_BUFFER_SET, binding = MODEL_BUFFER_BINDING) uniform DynamicUBO {
    mat4 model;
    RenderMaterial material;
};

layout(set = MODEL_SAMPLER_SET, binding = MODEL_SAMPLER_BINDING) uniform sampler textureSampler;

layout(set = MODEL_TEXTURE_SET, binding = MODEL_TEXTURE_BINDING) uniform texture2D textureImage;

const float kA = 0.02f;
const float kD = 0.49f;
const float kS = 0.49f;


float calculateShadow() {
    vec4 lightSpacePosition = vec4(light_space_position, 1.0f);
    lightSpacePosition.xyz /= lightSpacePosition.w;

    vec3 screenLocation = lightSpacePosition.xyz * 0.5 + 0.5;
    screenLocation.y = 1 - screenLocation.y; // Needs to be flipped for some reason?

    float net = 0.0f;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            // The closest depth from the perspective of the light source
            float closestLightDepth = texture(shadow_map, screenLocation.xy + vec2(i, j) / vec2(2560.f, 1440.f)).r;
            // Our current depth from the perspective of the light source
            float currentLightDepth = lightSpacePosition.z;

            float bias = max(0.05 * (1.0 - normal.y), 0.005);
            net += clamp(currentLightDepth - bias, 0.0f, 1.0f) > closestLightDepth ? 0.8 : 0.0;
        }
    }

//    if (currentLightDepth > 1.0) {
//        return 0.0;
//    }

    return net / (3.0f * 3.0f);
}

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
    float shadow = 1.0f - calculateShadow();

    vec3 image_base;
    if (material.kD_index >= 0) {
        image_base = texture(sampler2D(textureImage, textureSampler), uv).rgb;
    } else {
        image_base = material.kD.rgb;
    }

    color += vec4(material.kA);
    float multiplier =  pow(occlusion, 1.0f);
    vec4 base_color_occluded = vec4(multiplier * image_base, 1.0f);
    color = shadow * kA * base_color_occluded;

    for (int i = 0; i < light_count; i++) {
        vec4 light_position = positions[i];
        float intensity = positions[i].w;
        light_position.w = 1.f;
        vec4 light_pos = view * light_position;
        light_pos /= light_pos.w;

        vec3 diffuse_ray = light_pos.xyz - position;

        float attent = min(1 / (pow(length(position - light_pos.xyz), 2) + 0.01), 1);
        float diffuse = max(dot(normalize(normal), normalize(diffuse_ray)), 0);

        vec3 halfway = normalize((-position + diffuse_ray));

        float specular = max(dot(halfway, normal), 0);
        specular = pow(specular, 64.0f);
        
        // No attentuation
        if (intensity == 0) {
            intensity = 1.0f;
            attent = 1.0f;
        }

        color += intensity * shadow * attent * diffuse * kD * vec4(image_base, 0.0);
        color += intensity * shadow * attent * specular * kS * vec4(image_base + 0.1, 0);
    }

    color = color / (color + vec4(1));
}
