#version 150

//uniform vec3 colour;

in vec3 normal;

out vec4 outc;

const vec3 add = vec3(1,1,1);

void main() {
    //outc = vec4(1, 1, 1, 1);
    //outc = vec4(colour, 1);
    vec3 norm = (normal + add) / 2;
    outc = vec4(norm, 1);
}