#version 450

layout(location = 0) out float color;
layout(location = 1) in vec3 raw_normal;

layout(set = GLOBAL_UBO_SET, binding = GLOBAL_UBO_BINDING) uniform UBO {
    mat4 proj;
    mat4 view;
    vec3 samples[64];
};

layout(set = SSAO_DEPTH_SET, binding = SSAO_DEPTH_BINDING) uniform sampler2D depthSampler;
layout(set = SSAO_NOISE_SET, binding = SSAO_NOISE_BINDING) uniform sampler2D noiseSampler;

vec2 screenSize = vec2(1, 1);
vec2 noiseScale = screenSize / vec2(4.0, 4.0);
const float radius = 0.5f;
const float bias = 0.005f;
const int kernelSize = 64;

vec3 getPosition(vec2 screenXY) {
    vec2 uv = screenXY / screenSize;
//    uv.y = 1 - uv.y;
    float depth = texture(depthSampler, uv).r;
    vec4 result = inverse(proj) * vec4(uv.x * 2.0f - 1.0f, uv.y * 2.0f - 1.f, depth, 1.0f);
    result.xyz /= result.w;
    return result.xyz;
}

vec3 getPositionDirect(vec2 uv) {
    float depth = texture(depthSampler, uv).r;
    vec4 result = inverse(proj) * vec4(uv.x * 2.0f - 1.0f, uv.y * 2.0f - 1.f, depth, 1.0f);
    result.xyz /= result.w;
    return result.xyz;
}

void main() {
    ivec2 raw_size = textureSize(depthSampler, 0);
    screenSize = vec2(raw_size.x, raw_size.y);
    noiseScale = screenSize / vec2(4.0, 4.0);
    vec2 screenXY = gl_FragCoord.xy;
    vec2 uv = gl_FragCoord.xy / screenSize;
    vec3 position = getPosition(screenXY);
    vec3 normal = -raw_normal;
    vec3 randomVec = texture(noiseSampler,  uv * noiseScale).xyz;
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 samplePos = TBN * samples[i]; // from tangent to view-space
        samplePos = position + samplePos * radius;

        vec4 offset = vec4(samplePos, 1.0);
        offset      = proj * offset;    // from view to clip-space
        offset.xyz /= offset.w;               // perspective divide
        offset.xyz  = offset.xyz * 0.5 + 0.5;
        float sampleDepth = getPositionDirect(offset.xy).z;
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(position.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    color = 1 - occlusion / kernelSize;
}
