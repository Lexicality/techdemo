//
//  main.cpp
//  TechDemo
//
//  Created by Lexi Robinson on 18/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
// GLFW
#define GLFW_INCLUDE_GL3
#define GLFW_NO_GLU
#include <GL/glfw.h>
// GLM libs
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Various hardcoded options
#define TRIANGLE_DEBUGGING


void opencontext();
void loadshit();
void renderloop();
void shutdowneverything();

glm::mat4 projectionMatrix;

int main(int argc, const char * argv[])
{
    try {
        opencontext();
        loadshit();
        renderloop();
    } catch (std::exception& e) {
        // Place breakpoint here!
        std::cerr << "Uncaught exception!\n" << e.what() << std::endl;
    }
    shutdowneverything();
    return 0;
}


void opencontext() {
    // Initialise GLFW
    if(!glfwInit()) {
        throw std::runtime_error("Failed to initialise GLFW!");
    }
    // Make sure we de-GLFW.
    std::atexit(glfwTerminate);
    // Setup the context
    glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
    glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // TODO: Some kind of proper size detectione
    int windowWidth  = 1280; // 1 standard resolution step smaller than my mbp's screen.
    int windowHeight = 800;
    // Open a window
    if(!glfwOpenWindow(windowWidth, windowHeight, 0, 0, 0, 0, 32, 0, GLFW_WINDOW)) {
        throw std::runtime_error("Failed to open GLFW window");
    }
    // Name it
    glfwSetWindowTitle("Gaudy Tech Demo");
    // OpenGL
    
    // Heh heh heh cornflowers.
    glClearColor(100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 0.0f);
    
    // Depth
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    
    // Alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
#ifdef TRIANGLE_DEBUGGING
    // Debuging
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif

    
    // Projectione
    projectionMatrix = glm::perspective(75.0f,  // FoV
        static_cast<GLfloat> (windowWidth) /    // Aspect ratio
        static_cast<GLfloat> (windowHeight),
        0.1f, 100.0f                            // nearZ, farZ
    );
}

void nextline(std::istream& str) {
    const auto everything = std::numeric_limits<std::streamsize>::max();
    str.ignore(everything, '\n');
}
bool whitespace(const char c) {
    return c == ' ' || c == '\t';
}
bool newline(const char c) {
    return c == '\n' || c == '\r';
}
void loadshit() {
    using std::cout;
    using std::endl;
    std::ifstream file;
    file.exceptions(std::ios::failbit | std::ios::badbit);
    file.open("teapot.obj");
    
    std::vector<float> poses;
    std::vector<float> normals;
    
    char in;
    float f;
    std::string last = "";
    while (!file.eof()) {
        file.get(in);
        if (in == '#') {
            nextline(file);
            continue;
        } else if (whitespace(in) || newline(in))
            // ignore initial whitespace.
            continue;
        // start reading today's command
        std::ostringstream str;
        while(!whitespace(in)) {
            str << in;
            file.get(in);
        }
        // got it!
        std::string cmd = str.str();
        if (cmd == "g") {
            // TODO: groups are not implemented yet!
        } else if (cmd == "v" || cmd == "vn") {
            for (int i = 0; i < 3; ++i) {
                file >> f;
                if (cmd == "v")
                    poses.push_back(f);
                else
                    normals.push_back(f);
            }
        } else {
            // ain doin shit yet
            if (cmd != last) cout << endl;
            cout << cmd;
            last = cmd;
        }
        nextline(file);
    }
    cout << endl << "got " << poses.size() << " vertexes and " << normals.size() << " normals!" << endl;
}

void renderloop() {
    glClear(GL_COLOR_BUFFER_BIT);
    while (glfwGetKey('Q') != GLFW_PRESS)
        glfwSwapBuffers();
    // We aint loadin nowt
}
void shutdowneverything() {
    // Nothing actually needs to die (yet!). GLFW will die on it own.
}