#version 460 core
#extension GL_EXT_debug_printf: enable

layout (push_constant, std430) uniform coordinate_data {
        mat4 model;
        mat4 camera;
        mat4 proj;
};

layout (location = 0) in vec3 vertexP;
layout (location = 1) in vec2 vertexUV;
layout (location = 2) in vec3 vertexColor;

layout(location = 0) out vec3 vColor;

void main() {
        // the W is what defines what is clipped. -w <= x, y, z <= w
        gl_Position = model * camera * proj * vec4(vertexP, 1.0);
        vColor = vertexColor;
}
