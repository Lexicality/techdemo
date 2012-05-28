#version 150

in vec3 pos;

uniform mat4 View;
uniform mat4 Projection;

void main() {
    gl_Position = 
    View *
    Projection * 
    vec4(pos, 1);
}