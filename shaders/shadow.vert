#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 tangent;
layout(location = 4) in uint materialIndex;

layout(location = 0) out vec3 viewSpacePosition;
layout(location = 1) out vec3 outputNormal;
layout(location = 2) out vec2 outputUV;
layout(location = 3) out vec3 outputTangent;
layout(location = 4) out vec3 lightSpacePosition;
layout(location = 5) out uint outputMaterialIndex;

/*
#ifndef GLOBAL_UBO_BINDING
#define GLOBAL_UBO_BINDING 0
#endif

#ifndef MODEL_BUFFER_BINDING
#define MODEL_BUFFER_BINDING 1
#endif
*/

layout(set = SHADOW_UBO_SET, binding = SHADOW_UBO_BINDING) uniform UBO {
    mat4 proj;
    mat4 view;
    mat4 lightView;
};

layout(set = MODEL_BUFFER_SET, binding = MODEL_BUFFER_BINDING) uniform DynamicUBO {
    mat4 model;
};

void main() {
    gl_Position = proj * view * model * vec4(position, 1.0);
    viewSpacePosition = vec3(view * model * vec4(position, 1.0));
    lightSpacePosition = vec3(lightView * model * vec4(position, 1.0));
    vec4 rawOutputNormal = transpose(inverse(view * model)) * vec4(normal, 1.0);
    outputNormal = normalize(vec3(rawOutputNormal /= rawOutputNormal.w));
    outputUV = uv;
    outputTangent = tangent;
    outputMaterialIndex = materialIndex;
}
