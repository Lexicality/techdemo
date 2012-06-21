#version 150
#extension GL_ARB_explicit_attrib_location : require
//
// TechDemo/Shaders/BasicVertex.vert
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;

out vec3 normal;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main() {
    gl_Position = 
    Projection * 
    View *
    Model *
    vec4(pos, 1);
    normal = norm;
}