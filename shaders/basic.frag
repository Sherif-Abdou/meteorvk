#version 450

layout(location = 0) out vec4 color;

layout(binding = 1) uniform sampler2D depthSampler;

void main() {
    float sampled = texture(depthSampler, vec2(gl_FragCoord.xy) / vec2(1280, 800)).r;
    color = vec4(0.9, 0.4, pow(sampled, 256), 1.0);
}