//
// TechDemo/ResourceManager.h
// Copyright (C) 2012 Lexi Robinson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#pragma once
#include <vector>
#include <stdexcept>
#include "gl.h"

#include "Constants.h"



namespace OpenGL {
    typedef std::vector<GLuint> GLuintVector;
    CREATE_EXCEPTION(CorruptShaderException, std::runtime_error);
    CREATE_EXCEPTION(NoSuchShaderException,  std::runtime_error);
    CREATE_EXCEPTION(NoSuchTextureException, std::runtime_error);
    class ResourceManager {
        
    public:
        ResourceManager();
        ResourceManager(ResourceManager&& other);
        ~ResourceManager();

        GLuint CreateVBO();
        GLuint CreateVAO();

        GLuintVector CreateVBOs(GLsizei n);
        GLuintVector CreateVAOs(GLsizei n);

        GLuint LoadShaders(const std::string& name);
        GLuint LoadShaders(const std::string& vertex, const std::string& fragment);
        GLuint LoadShaders(const std::string& vertex, const std::string& geometry, const std::string& fragment);
        GLuintVector LoadShaders(GLsizei n, const std::string& name);
        GLuintVector LoadShaders(GLsizei n, const std::string& vertex, const std::string& fragment);
        GLuintVector LoadShaders(GLsizei n, const std::string& vertex, const std::string& geometry, const std::string& fragment);

        GLuint LoadTextureArray (const std::string& path);
        GLuint LoadTextureArray (const std::string& path, const short int start, const short int end);
        GLuint LoadSingleTexture(const std::string& path, GLuint *const width = nullptr, GLuint *const height = nullptr);

    private:
        GLuint genericTextureLoad(std::vector<std::string>& files);

        GLuintVector vbos;
        GLuintVector vaos;
        GLuintVector programs;
        GLuintVector textures;

    };
}
