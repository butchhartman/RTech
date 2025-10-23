#version 460 core

vec3 vertexPosition[3] = vec3[3](
        vec3(0.0, 0.5, 1.0),
        vec3(-0.5, -0.5, 1.0),
        vec3(0.5, -0.5, 1.0)
);

void main() {
        gl_Position = vec4(vertexPosition[gl_VertexIndex], 1.0);
}
