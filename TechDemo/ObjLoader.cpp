//
//  ObjLoader.cpp
//  TechDemo
//
//  Created by Lexi Robinson on 28/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//
#include "ObjLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include "PhysFS.h"

void nextline(std::istream& str);
bool whitespace(const char c);
bool newline(const char c);

GLsizei LoadObj(const std::string& name, const GLuint VAO, const GLuint VAB, const GLuint EAB) {
    if (!PhysFS::exists(name))
        throw std::runtime_error("Can't find the file " + name);
    using std::cout;
    using std::endl;
	PhysFS::FileStream file(name, PhysFS::OM_READ);
    file.exceptions(std::ios::failbit | std::ios::badbit);
    //file.open(name);
    
    std::vector<float> poses;
    std::vector<float> normals;
    std::vector<unsigned short> indicies;
    
    char in;
    float f;
    unsigned short s;
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
        } else if (cmd == "f") {
            // TODO: Support the / formats.
            for (int i = 0; i < 3; ++i) {
                file >> s;
                indicies.push_back(s);
            }
        } else {
            // ain doin shit yet
            if (last == "") cout << '\'';
            else if (cmd != last) cout << '\'' << endl << '\'';
            cout << cmd;
            last = cmd;
        }
        nextline(file);
    }
    if (last != "")
        cout << '\'' << endl;
    
    size_t numv = poses.size();
    size_t numvn = normals.size();
    size_t numf = indicies.size();
    cout << "got " << numv << " vertexes and " << numvn << " normals with " << numf << " faces using them!" << endl;
    /*
     * Vertacies
     */
    glBindBuffer(GL_ARRAY_BUFFER, VAB);
    size_t floatsize = sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, (numv /*+ numvn*/) * floatsize, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numv * floatsize, &poses[0]);
    //glBufferSubData(GL_ARRAY_BUFFER, numv * floatsize, numvn * floatsize, &normals[0]);
    
    /*
     * Indacies
     */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EAB);
    size_t shortize = sizeof(unsigned short);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numf * shortize, &indicies[0], GL_STATIC_DRAW);
    
    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);
    //glEnableVertexAttribArray(1); /*
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
    /*/
     glVertexAttribPointer(0, 3, GL_FLOAT, false, 3, (void*)(            0));
     glVertexAttribPointer(1, 3, GL_FLOAT, false, 3, (void*)(floatsize * 3));
     //*/
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return numf;
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
