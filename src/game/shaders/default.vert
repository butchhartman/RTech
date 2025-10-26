#version 460 core
#extension GL_EXT_debug_printf: enable
layout (location = 0) in vec3 vertexP;


void main() {
        debugPrintfEXT("Vertex: %f, %f, %f", vertexP.x, vertexP.y, vertexP.z);
        gl_Position = vec4(vertexP, 1.0);
}
