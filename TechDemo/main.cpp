//
//  main.cpp
//  TechDemo
//
//  Created by Lexi Robinson on 18/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <stdexcept>
// GL
#include "gl.h"
// GLFW
#include <GL/glfw.h>
// GLM libs
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// Personal libs
#include "PhysFS.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "ObjLoader.h"

// Various hardcoded options
#define TRIANGLE_DEBUGGING


// loadin
void initphysfs(const char *argv0);
void loadshit();
void loadobj(const std::string& name);
// renderin
void opencontext();
void render();
void errchck(const char* str);
bool checkwindow();
// movin
void processmove(const InputData& in, const double deltatime);
// closedown
void shutdowneverything();

// woo globals
GLuint VAO, VAB, EAB;
GLuint program;
GLuint viewLocation;
GLsizei numpoints;

OpenGL::ResourceManager *mgr;

bool viewUpdated;

glm::mat4      modelMatrix;
glm::mat4       viewMatrix;
glm::mat4 projectionMatrix;

int main(int argc, const char * argv[])
{
    try {
        initphysfs(argv[0]);
        opencontext();
        loadshit();
        InputManager mgr;
        double ltime, ctime, dtime;
        while (checkwindow()) {
            // Time since last frame
            ctime = glfwGetTime();
            dtime = ctime - ltime;
            ltime = ctime;
            const auto& in = mgr.GetInput();
            if (in.Exit)
                break;
            processmove(in, dtime);
            render();
            usleep(1000);
        }
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
    const int windowWidth  = 1280; // 1 standard resolution step smaller than my mbp's screen.
    const int windowHeight = 800;
    // Open a window
    if(!glfwOpenWindow(windowWidth, windowHeight, 0, 0, 0, 0, 32, 0, GLFW_WINDOW)) {
        throw std::runtime_error("Failed to open GLFW window");
    }
    // Name it
    glfwSetWindowTitle("Gaudy Tech Demo");
    
#ifndef __APPLE__
#error TODO: GLEW!
#endif
    
    /*
     * OpenGL
     */
    
    // Heh heh heh cornflowers.
    glClearColor(100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 0.0f);
    
    // Depth
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glEnable(GL_CULL_FACE);
    
    // Alpha
    /*
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    */
#ifdef TRIANGLE_DEBUGGING
    // Debuging
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
    
    // Points
    glPointSize(50);
    
    // resource mgmnt
    mgr = new OpenGL::ResourceManager();
    
    // Projectione
    projectionMatrix = glm::perspective(75.0f,  // FoV
        static_cast<GLfloat> (windowWidth) /    // Aspect ratio
        static_cast<GLfloat> (windowHeight),
        0.1f, 100.0f                            // nearZ, farZ
    );
    modelMatrix = glm::scale(glm::mat4(1), glm::vec3(0.1));
    //viewMatrix  = glm::lookAt(glm::vec3(10, 0, 0), glm::vec3(10,0,1), glm::vec3(0,1,0));
    viewUpdated = true;
}

void loadshit() {
    VAO = mgr->CreateVAO();
    VAB = mgr->CreateVBO();
    EAB = mgr->CreateVBO();
    
    program = mgr->LoadShaders("BasicVertex", "BasicFragment");
    glUseProgram(program);
    
    viewLocation = glGetUniformLocation(program, "View");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    
    glUniformMatrix4fv(glGetUniformLocation(program, "Projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(glGetUniformLocation(program, "Model"     ), 1, GL_FALSE, glm::value_ptr(     modelMatrix));
    
    glUseProgram(0);
    numpoints = LoadObj("teapot.obj", VAO, VAB, EAB);
}

bool checkwindow() {
	return glfwGetWindowParam(GLFW_OPENED) == GL_TRUE;
}
            

void render() {
    if (!viewUpdated)
        return;
    viewUpdated = false;
    glUseProgram(program);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glBindVertexArray(VAO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // hum
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EAB);
    glDrawElements(GL_TRIANGLES, numpoints, GL_UNSIGNED_SHORT, 0);
    glfwSwapBuffers();
}
void shutdowneverything() {
    delete mgr;
}


void processmove(const InputData& input, const double dtime) {
    static const float fMouseSpeed = 0.02f;
    static const float fKeySpeed   = 6.0f;
	//static const float clampUp = 3.14f * 2.2f;
	//static const float clampDn = 3.14f * 1.8f;
    
    static       float viewAngleH, viewAngleV;
    static       glm::vec3 viewPos;
    
	const float ftime = static_cast<float>(dtime);
    
    if (input.DeltaViewX == 0 && input.DeltaViewY == 0 && !(
        input.Forwards || input.Backwards || input.Left || input.Right))
        return;
    
	// Viewage
	viewAngleH += fMouseSpeed * input.DeltaViewX;
	viewAngleV -= fMouseSpeed * input.DeltaViewY;
	//viewAngleV = (viewAngleV > clampUp) ? clampUp : viewAngleV;
	//viewAngleV = (viewAngleV < clampDn) ? clampDn : viewAngleV;
    
    glm::vec3 dir(
                 sin(viewAngleH) * cos(viewAngleV),
                 sin(viewAngleV),
                 cos(viewAngleH) * cos(viewAngleV)
                 );
	float fhvang = viewAngleH - 3.14f / 2.0f;
	glm::vec3 right (
                     sin(fhvang),
                     0,
                     cos(fhvang)
                     );
    glm::vec3 up = glm::cross(dir, right);
	glm::vec3 vel(0,0,0);
	float amt = ftime * fKeySpeed;
	if (input.Forwards)
		vel += dir * amt;
	if (input.Backwards)
		vel -= dir * amt;
	if (input.Left)
		vel += right * amt;
	if (input.Right)
		vel -= right * amt;
	viewPos += vel;
	viewMatrix = glm::lookAt(viewPos, viewPos + dir, up);
    viewUpdated = true;
}

/*
 * PhysFS
 */

void killphysfs() {
	try {
		PhysFS::deinit();
	} catch(PhysFS::Exception) {}
}
void initphysfs(const char *argv0) {
    try {
		PhysFS::init(argv0);
		PhysFS::mount("./", "", false);
        std::atexit(killphysfs);
	} catch (PhysFS::Exception& e) {
        std::cerr << "Could not start PhysFS! " << e.what() << std::endl;
        throw;
	}
}

void errchck(const char* str) {
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cerr << "fuckup in " << str << std::endl;
		if (err == GL_INVALID_ENUM)
			std::cerr << "INVALID ENUM!" << std::endl;
		else if (err == GL_INVALID_VALUE)
			std::cerr << "Invalid value!" << std::endl;
		else if (err == GL_INVALID_OPERATION)
			std::cerr << "Invalid opreation!" << std::endl;
		else if (err == GL_INVALID_FRAMEBUFFER_OPERATION)
			std::cerr << "whoops, there goes the framebuffer" << std::endl;
		else if (err == GL_OUT_OF_MEMORY)
			std::cerr << "ABORT ABORT" << std::endl;
	}
}