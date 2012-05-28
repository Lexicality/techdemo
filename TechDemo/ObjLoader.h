//
//  ObjLoader.h
//  TechDemo
//
//  Created by Lexi Robinson on 28/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//
#pragma once
#include "gl.h"
#include <string>

GLsizei LoadObj(const std::string& name, const GLuint VAO, const GLuint VAB, const GLuint EAB);