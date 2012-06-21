#version 150
//
// TechDemo/Shaders/BasicFragment.frag
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
in vec3 normal;

out vec4 outc;

const vec3 add = vec3(1,1,1);
const vec3 lighting = vec3(1, 1, 1);

void main() {
    //outc = vec4(1, 1, 1, 1);
    //outc = vec4(colour, 1);
    //*
    vec3 norm = (normal + add) / 2;
    outc = vec4(norm, 1);
    /*/
    float amt = (dot(lighting, normal) + 1) / 2;
    if (amt < 0.2)
        amt = 0.2;
    //if (amt > 0.9)
    //   amt = 0.9;
    outc = vec4(amt, amt, amt, 1);
    // */
}