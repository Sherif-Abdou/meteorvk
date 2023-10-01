#version 450

layout(location = 0) out vec4 color;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 tangent;

layout(binding = 0) uniform UBO {
    mat4 proj;
    mat4 view;
    mat4 model;
    mat4 lightProjView;
};

layout(binding = 1) uniform sampler2D depthSampler;

vec4 lightPosition = vec4(5, 5, 0, 1);

const vec3 albedo = vec3(0.054901960784313725, 0.4549019607843137, 0.5647058823529412);
const float kA = 0.3f;
const float kD = 0.8f;

float calculateShadow() {
    vec4 lightSpacePosition = lightProjView * inverse(view) * vec4(position, 1.0f);
    lightSpacePosition.xyz /= lightSpacePosition.w;

    vec3 screenLocation = lightSpacePosition.xyz * 0.5 + 0.5;
    float closestLightDepth = texture(depthSampler, screenLocation.xy).r;
    float currentLightDepth = lightSpacePosition.z;

    float bias = max(0.05 * (1.0 - normal.y), 0.005);

    if (currentLightDepth > 1.0) {
        return 0.0;
    }

    return currentLightDepth - bias > closestLightDepth ? 0.8 : 0.0;
}

void main() {
    float shadow = calculateShadow();

    lightPosition = view * lightPosition;
    lightPosition /= lightPosition.w;
    float diffuse = max(dot(normalize(normal) , normalize(lightPosition.xyz-position)), 0);

    color = vec4(diffuse * (1-shadow) * kD * albedo + kA * albedo, 1.0);
}