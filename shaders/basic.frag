#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) out vec4 color;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 light_space_position;
layout(location = 5) flat in uint material_id;

const float PI = 3.14159265359;

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

// PBR methods from learnopengl.com
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}



void main() {
    float metallic = 0.0f;
    vec3 sampledAlbedo = vec3(0.0);
    vec3 sampledAmbient = vec3(0.0);
    float nS = 8.0;
    if (material_id != -1) {
        sampledAlbedo = materials[material_id].kD;
        sampledAmbient = materials[material_id].kA;
        specularColor = materials[material_id].kS;
        if (materials[material_id].illum != 2) {
            kS = 0.0f;
            metallic = 1.0f;
        }
//        int index = materials[material_id].kD_index;
//        if (index != -1) {
//
//            sampledAlbedo = texture(sampler2D(textures[index], s[0]), uv).rgb;
//        }
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
    vec2 adjustedUV = uv / vec2(material.bounds.x2 - material.bounds.x1, material.bounds.y2 - material.bounds.y1);
    adjustedUV += vec2(material.bounds.x1, material.bounds.y1);

    lightPosition = view * lightPosition;
    lightPosition /= lightPosition.w;
    vec3 light = normalize(lightPosition.xyz - position);
    float diffuse = max(dot(normalize(normal) , light), 0);
    vec3 view = normalize(-position);
    vec3 halfway = normalize(view + light);
//    float specular = pow(max(dot(halfway, normal), 0), nS);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, sampledAlbedo, metallic);
    vec3 F  = fresnelSchlick(max(dot(halfway, view), metallic), F0);

    float NDF = DistributionGGX(normal, halfway, nS);
    float G   = GeometrySmith(normal, view, light, nS);

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, view), 0.0) * max(dot(normal, light), 0.0)  + 0.0001;
    vec3 specular     = numerator / denominator;

    vec3 S = F;
    vec3 D = vec3(1.0) - S;

    D *= 1.0 - metallic;


    float shadow = calculateShadow();
    float occlusion = getOcclusion();

    float NdotL = max(dot(normal, light), 0.0);
    vec3 radiance = vec3(1.0f);

    vec3 ambient = vec3(0.3) * sampledAlbedo * occlusion;

    color.rgb = (D * sampledAlbedo * (1.0 - shadow) / PI + specular) * radiance * NdotL;
    color.rgb += ambient;



//    color = vec4(diffuse * (1-shadow) * kD * sampledAlbedo
//        + kA * (occlusion) * sampledAmbient
//        + kS * (1.0f) * specular * specularColor
//        , 1.0);
    color.rgb = color.rgb / (color.rgb + vec3(1.0));
}
