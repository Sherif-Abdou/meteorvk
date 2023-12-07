#version 450

layout(location = 0) out vec4 color;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 light_space_position;

layout(binding = 0) uniform UBO {
    mat4 proj;
    mat4 view;
    mat4 lightProjView;
};

struct AtlasRect {
    float x1;
    float x2;
    float y1;
    float y2;
};

struct Material {
    vec4 albedo;
    AtlasRect bounds;
};

layout(binding = 2) uniform DynamicUBO {
    mat4 _model;
    Material material;
};

layout(binding = 1) uniform sampler2D depthSampler;

layout(binding = 3) uniform sampler2D textureSampler;

layout(binding = 4) uniform sampler2D occlusionSampler;

vec4 lightPosition = view * vec4(-5, 5, 2, 1);

//const vec3 albedo = vec3(0.054901960784313725, 0.4549019607843137, 0.5647058823529412);
const float kA = 0.5f;
const float kD = 0.8f;

const vec3 specularColor = vec3(0.7);

float calculateShadow() {
    vec4 lightSpacePosition = vec4(light_space_position, 1.0f);
    lightSpacePosition.xyz /= lightSpacePosition.w;

    vec3 screenLocation = lightSpacePosition.xyz * 0.5 + 0.5;
    screenLocation.y = 1 - screenLocation.y; // Needs to be flipped for some reason?
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
    vec2 screenUV = gl_FragCoord.xy / vec2(2560, 1440);
    screenUV.x = 1 - screenUV.y;
    vec2 adjustedUV = uv / vec2(material.bounds.x2 - material.bounds.x1, material.bounds.y2 - material.bounds.y1);
    adjustedUV += vec2(material.bounds.x1, material.bounds.y1);

    lightPosition = view * lightPosition;
    lightPosition /= lightPosition.w;
    float diffuse = max(dot(normalize(normal) , normalize(lightPosition.xyz-position)), 0);
    vec3 halfway = normalize(-position + (lightPosition.xyz - position));
    float specular = pow(max(dot(halfway, normal), 0), 8);

    vec4 albedo = material.albedo.a == 0.0 ? texture(textureSampler, adjustedUV) : material.albedo;
    float occlusion = texture(occlusionSampler, screenUV).r;

    color = vec4(diffuse * (1-shadow) * kD * albedo.rgb + kA * (occlusion) * albedo.rgb + (1-kD) * specular * specularColor, 1.0);
    color = vec4(vec3(occlusion), 1.0f);
}