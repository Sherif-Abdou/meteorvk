#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) out vec4 color;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 light_space_position;
layout(location = 5) flat in uint material_id;

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
    int textureIndex;
    int samplerIndex;
};

struct RenderMaterial {
    vec3 kA;
    vec3 kD;
    vec3 kS;
    float nS;
    uint illum;
    int kD_index;
    int kS_index;
};

layout(binding = 2) uniform DynamicUBO {
    mat4 _model;
    Material material;
};

layout(binding = 1) uniform sampler2D depthSampler;

layout(binding = 3) uniform sampler2D textureSampler;

layout(binding = 4) uniform sampler2D occlusionSampler;

layout(set = 1, binding=0) uniform sampler s[8];
layout(set = 1, binding=1) uniform texture2D textures[100];

layout(set = 1, binding=2) buffer materialBuffer {
    RenderMaterial materials[];
};

vec4 lightPosition = view * 1 * vec4(-0, 5, 0, 1);

//const vec3 albedo = vec3(0.054901960784313725, 0.4549019607843137, 0.5647058823529412);
float kA = 0.0f;
float kD = 0.7f;
float kS = 0.0f;

vec3 specularColor = vec3(0.7);

float calculateShadow() {
    vec4 lightSpacePosition = vec4(light_space_position, 1.0f);
    lightSpacePosition.xyz /= lightSpacePosition.w;

    vec3 screenLocation = lightSpacePosition.xyz * 0.5 + 0.5;
    screenLocation.y = 1 - screenLocation.y; // Needs to be flipped for some reason?

    float net = 0.0f;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            float closestLightDepth = texture(depthSampler, screenLocation.xy + vec2(i, j) / vec2(1280, 720)).r;
            float currentLightDepth = lightSpacePosition.z;

            float bias = max(0.05 * (1.0 - normal.y), 0.005);
            net += clamp(currentLightDepth - bias, 0.0f, 1.0f) > closestLightDepth ? 0.8 : 0.0;;
        }
    }

//    if (currentLightDepth > 1.0) {
//        return 0.0;
//    }

    return net / (3.0f * 3.0f);
}

float getOcclusion() {
    ivec2 size = textureSize(occlusionSampler, 0);
    vec2 screenUV = gl_FragCoord.xy / vec2(size.x, size.y);
    float net = 0.0f;
    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            vec2 adjusted = screenUV + (vec2(i, j) / vec2(size.x, size.y));
            float occlusion = texture(occlusionSampler, adjusted).r;
            net += occlusion / (5.0f * 5.0f);
        }
    }
    return pow(net, 1.0f);
}

void main() {
    vec3 sampledAlbedo = vec3(0.0);
    vec3 sampledAmbient = vec3(0.0);
    float nS = 8.0;
    if (material_id != -1) {
        sampledAlbedo = materials[material_id].kD;
        sampledAmbient = materials[material_id].kA;
        specularColor = materials[material_id].kS;
        if (materials[material_id].illum != 2) {
            kS = 0.0f;
        }
        nS = materials[material_id].nS;
    } else if (material.textureIndex != -1) {
        sampledAlbedo = texture(sampler2D(textures[material.textureIndex], s[material.samplerIndex]), uv).rgb;
        sampledAmbient = sampledAlbedo;
        specularColor = vec3(0.7);
    } else {
        sampledAlbedo = material.albedo.rgb;
        sampledAmbient = sampledAlbedo;
        specularColor = vec3(0.7);
    }
    float shadow = calculateShadow();
    vec2 adjustedUV = uv / vec2(material.bounds.x2 - material.bounds.x1, material.bounds.y2 - material.bounds.y1);
    adjustedUV += vec2(material.bounds.x1, material.bounds.y1);

    lightPosition = view * lightPosition;
    lightPosition /= lightPosition.w;
    float diffuse = max(dot(normalize(normal) , normalize(lightPosition.xyz-position)), 0);
    vec3 halfway = normalize(-position + (lightPosition.xyz - position));
    float specular = pow(max(dot(halfway, normal), 0), nS);

    vec4 albedo = material.albedo.a == 0.0 ? texture(textureSampler, adjustedUV) : material.albedo;
    float occlusion = getOcclusion();
    vec2 screenUV = gl_FragCoord.xy / vec2(2560, 1440);

    color = vec4(diffuse * (1-shadow) * kD * sampledAlbedo
        + kA * (occlusion) * sampledAmbient
        + kS * (1.0f) * specular * specularColor
        , 1.0);
    color.rgb = color.rgb / (color.rgb + vec3(1.0));
}