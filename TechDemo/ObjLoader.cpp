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
#include <map>
#include <utility>
#include <glm/glm.hpp>
#include "PhysFS.h"

typedef unsigned int    index_t;
typedef          int rawindex_t;
typedef std::tuple<index_t, index_t, index_t> indexid_t;

typedef std::tuple<float, float, float> normalkey_t;

void nextline(std::istream& str);
bool whitespace(const char c);
bool newline(const char c);

template <class A>
void dumpvector(const std::vector<A>& v);

void addindex(std::vector<index_t>& into, rawindex_t put, const size_t vsize);

rawindex_t readindex(PhysFS::FileStream& file);

indexid_t compileindex(index_t pos, index_t tex, index_t norm);

template <class A, class B>
bool isvalid(const std::map<A, B>& map, const typename std::map<A, B>::iterator& i);

void addvars(std::vector<float>& dest, const std::vector<float>& src, size_t start, size_t num = 3);

normalkey_t normalkey(float x, float y, float z);
normalkey_t normalkey(const glm::vec3& vec);

glm::vec3 getvec(const std::vector<float>& src, size_t start);

/**
 * Loads a .obj file of the specified name into GPU memory.
 * @param name The obj file
 * @param VAO  The Vertex Array Object to store the mesh info in
 * @param VAB  The Vertex Buffer Object to store the ARRAY_BUFFER in
 * @param EAB  The Vertex Buffer Object to store the ELEMENT_ARRAY_BUFFER in
 */
GLsizei LoadObj(const std::string& name, const GLuint VAO, const GLuint VAB, const GLuint EAB) {
    if (!PhysFS::exists(name))
        throw std::runtime_error("Can't find the file " + name);
    using std::cout;
    using std::endl;
	PhysFS::FileStream file(name, PhysFS::OM_READ);
    file.exceptions(std::ios::badbit);
    //file.open(name);
    
    
    std::vector<float> verts(3, 0);
    std::vector<float> texes(2, 0);
    std::vector<float> norms(3, 0);
    std::vector<index_t> vertind;
    std::vector<index_t> texpind;
    std::vector<index_t> normind;
    
    std::vector<index_t> verttemp;
    std::vector<index_t> texptemp;
    std::vector<index_t> normtemp;
    
    std::map<normalkey_t, index_t> normallookup;
    
    size_t numv = 0;
    size_t numvn = 0;
    
    size_t numf = 0;
    
    char in;
    float f, x, y, z;
    bool single, notex, nonorm, nonormslash;
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
        } else if (cmd == "v") {
            ++numv;
            for (int i = 0; i < 3; ++i) {
                file >> f;
                if (cmd == "v")
                    verts.push_back(f);
            }
        } else if (cmd == "vn") {
            ++numvn;
            file >> x;
            file >> y;
            file >> z;
            norms.push_back(x);
            norms.push_back(y);
            norms.push_back(z);
            normallookup[normalkey(x, y, z)] = numvn;
        } else if (cmd == "f") {
            ++numf;
            
            verttemp.clear();
            normtemp.clear();
            single = false;
            notex  = false;
            nonorm = false;
            nonormslash = false;
            addindex(verttemp, readindex(file), numv);
            in = file.peek();
            if (in != '/') {
                single = true;
            } else {
                file.ignore(1);
                in = file.peek();
                if (in == '/')
                    notex = true;
                else
                    // TODO: Tex coordinates
                    readindex(file);
                // If the next char is not a / then the file is technically broken.
                // Unfortunately, sponza_basic.obj does not conform to this view.
                in = file.peek();
                if (in == '/')
                    file.ignore(1);
                else
                    nonormslash = true;
                    
                in = file.peek();
                if (whitespace(in))
                    nonorm = true;
                else
                    addindex(normtemp, readindex(file), numvn);
            }
            // TODO: Support more than just trianlges
            for (int i = 0; i < 2; ++ i) {
                addindex(verttemp, readindex(file), numv);
                if (!single) {
                    file.ignore(1);
                    if (!notex)
                        // TODO: Tex coordinates
                        readindex(file);
                    if (!nonormslash)
                        file.ignore(1);
                    if (!nonorm)
                        addindex(normtemp, readindex(file), numvn);                    
                }
            }
            if (nonorm || single) {
                // grab the face normals in leu of vertex normals
                cout << "Deducing normals for " << verttemp[0] << ", " << verttemp[1] << ", "<< verttemp[2] << ": ";
                glm::vec3
                    a = getvec(verts, verttemp[0]),
                    b = getvec(verts, verttemp[1]),
                    c = getvec(verts, verttemp[2]);
                glm::vec3 norm = glm::normalize(glm::cross(c - a, b - a));
                cout << "(" << norm.x << "," << norm.y << "," << norm.z << "). ";
                auto key = normalkey(norm);
                const auto existing = normallookup.find(key);
                index_t ind;
                if (isvalid(normallookup, existing)) {
                    ind = existing->second;
                    cout << "This is the existing normal with index: ";
                } else {
                    numvn++;
                    ind = numvn;
                    norms.push_back(norm.x);
                    norms.push_back(norm.y);
                    norms.push_back(norm.z);
                    normallookup[key] = numvn;
                    cout << "This is a new normal, now assinged index: ";
                }
                cout << ind << endl;
                normtemp.push_back(ind);
                normtemp.push_back(ind);
                normtemp.push_back(ind);
            }
            vertind.insert(vertind.end(), verttemp.begin(), verttemp.end());
            normind.insert(normind.end(), normtemp.begin(), normtemp.end());
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
    cout << "got " << numv << " vertexes and " << numvn << " normals with " << numf  << " faces using them!" << endl;
    //*
    cout << "pos ind:" << endl;
    dumpvector(vertind);
    cout << "norm ind:" << endl;
    dumpvector(normind);
    // */
    
    // TODO: Compile a list of position indicies and normal indicies
    //        and use that to fill the various buffers.
    std::vector<float> positions(3, 0);
  //std::vector<float> texures(2, 0);
    std::vector<float> normals(3, 0);
    positions.reserve(verts.size());
  //textures .reserve(texes.size());
    normals  .reserve(norms.size());
    std::vector<index_t> indicies;
    std::map<indexid_t, index_t> indmap;
    index_t numinds = 0;
    auto pos = vertind.begin(), norm = normind.begin(), end = vertind.end();
    for (; pos != end; ++pos, ++norm) {
        //std::cout << "pos: " << *pos << " norm: " << *norm;
        auto ind = compileindex(*pos, 0, *norm);
        auto loc = indmap.find(ind);
        if (isvalid(indmap, loc)) {
            //std::cout << " found: " << loc->second;
            indicies.push_back(loc->second);
        } else {
            ++numinds;
            addvars(positions, verts, *pos);
          //addvars(textures,  texes, *tex);
            addvars(normals,   norms, *norm);
            indicies.push_back(numinds);
            indmap.insert(std::make_pair(ind, numinds));
            //std::cout << " new: " << numinds;
        }
        //std::cout << std::endl;
    }
    /*
    cout << "indicies:" << endl;
    dumpvector(indicies);
    // */   
    
    /*
     * Vertacies
     */
    numv  = positions.size();
  //numt  = textures .size();
    numvn = normals  .size();
    size_t floatsize = sizeof(float);
    size_t arrsize   = (numv /*+ numt*/ + numvn) * floatsize;
    size_t varrsize  =  numv                     * floatsize;
    //size_t tarrsize  =         numt            * floatsize;
    size_t vnarrsize =                    numvn  * floatsize;
    // TODO: put the rest of the commented out texture shit in here.
    const auto vertpointer = &positions[0];
    const auto normpointer = &normals  [0];
    
    glBindBuffer(GL_ARRAY_BUFFER, VAB);
    glBufferData(GL_ARRAY_BUFFER, arrsize, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, varrsize, vertpointer);
    glBufferSubData(GL_ARRAY_BUFFER, varrsize, vnarrsize, normpointer);
    
    /*
     * Indacies
     */
    size_t numi = indicies.size();
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EAB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numi * sizeof(index_t), &indicies[0], GL_STATIC_DRAW);
    
    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (void*)(               0));
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, (void*)(numv * floatsize));
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return numi;
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

template <class A>
void dumpvector(const std::vector<A>& v) {
    using std::cout;
    using std::endl;
    for (auto i = v.begin(), e = v.end(); i != e; ++i) {
        cout << *i << ", " << *(++i) << ", "  << *(++i) << endl;
    }
}

void addindex(std::vector<index_t>& into, rawindex_t put, const size_t vsize) {
    if (put < 0)
        // Negative values are relative positions
        put += vsize + 1;
    into.push_back(put);
}

rawindex_t readindex(PhysFS::FileStream& file) {
    rawindex_t s;
    file >> s;
    return s;
}

indexid_t compileindex(index_t pos, index_t tex, index_t norm) {
    return std::make_tuple(pos, tex, norm);
}

template <class A, class B>
bool isvalid(const std::map<A, B>& map, const typename std::map<A, B>::iterator& i) {
    return i != map.end();
}

void addvars(std::vector<float>& dest, const std::vector<float>& src, size_t start, size_t num) {
    start *= num;
    for (int i = 0; i < num; ++i)
        dest.push_back(src.at(start + i));
}

glm::vec3 getvec(const std::vector<float>& src, size_t start) {
    start *= 3;
    return glm::vec3(src.at(start), src.at(start + 1), src.at(start + 2));
}

normalkey_t normalkey(float x, float y, float z) {
    return std::make_tuple(x, y, z);
}

normalkey_t normalkey(const glm::vec3& vec) {
    return std::make_tuple(vec.x, vec.y, vec.z);
}










