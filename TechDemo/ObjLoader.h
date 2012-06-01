//
//  ObjLoader.h
//  TechDemo
//
//  Created by Lexi Robinson on 28/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//
#pragma once
#include <vector>
#include "gl.h"
#include "ResourceManager.h"

struct RenderData {
    GLuint  EAB;
    GLuint  VAO;
    GLsizei IndexCount;
    GLenum  IndexType;
};

std::vector<RenderData> LoadObj(const std::string& name, OpenGL::ResourceManager& mgr);