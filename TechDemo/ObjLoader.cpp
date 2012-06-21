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

template <class A>
void dumpvector(const std::vector<A>& v);

bool whitespace(const char c);
bool newline(const char c);

class ObjLoader {
    // Typing lazyness
    typedef unsigned int    index_t;
    typedef          int rawindex_t;
    typedef std::tuple<index_t, index_t, index_t> indexid_t;
    typedef std::tuple<float, float, float> normalkey_t;
    
    struct FaceMode {
        bool PositionsOnly;
        bool NoTextures;
        bool NoNormals;
        bool NoNormalSlash;
    };
    struct VertexData {
        index_t *positions;
        index_t *textures;
        index_t *normals;
    };

    // File operations
    void nextline();
    index_t readindex(size_t existing_count);

    // vector modding
    void addvars(std::vector<float>& dest, const std::vector<float>& src, size_t start, size_t num = 3) const;
    
    // Key compilation
    indexid_t compileindex(index_t pos, index_t tex, index_t norm) const;
    normalkey_t normalkey(float x, float y, float z) const;
    normalkey_t normalkey(const glm::vec3& vec) const;
    
    // Util
    glm::vec3 getvec(const std::vector<float>& src, size_t start) const;
    
    // Vars
    OpenGL::ResourceManager& mgr;
    std::stringstream file;
    std::vector<float>  
        loadedPositions,
        loadedTextures,
        loadedNormals;
    std::vector<index_t>
        positionIndicies,
        textureIndicies,
        normalIndicies;
    std::map<normalkey_t, index_t>
        normalsReverseLookup;
    size_t
        trianglesCount,
        positionCount,
        textureCount,
        normalCount;
    std::vector<RenderData> objects;
    
    // Meat n veg
    std::string GetCommand();
    void ReadPosition();
    void ReadNormal();
    void ReadFace();
    bool ReadVertex(const FaceMode& mode, const VertexData& verts, const size_t index);
    
    RenderData MakeData();
    // public shizzl
public:
    ObjLoader(const std::string& name, OpenGL::ResourceManager& mgr);
    std::vector<RenderData> GetData() const;
};

/**
 * Loads a .obj file of the specified name into GPU memory.
 * @param name The obj file
 * @param VAO  The Vertex Array Object to store the mesh info in
 * @param VAB  The Vertex Buffer Object to store the ARRAY_BUFFER in
 * @param EAB  The Vertex Buffer Object to store the ELEMENT_ARRAY_BUFFER in
 */
std::vector<RenderData> LoadObj(const std::string& name, OpenGL::ResourceManager& mgr) {
    ObjLoader loader(name, mgr);
    return loader.GetData();
}

std::vector<RenderData> ObjLoader::GetData() const{
    return objects;
}
    
ObjLoader::ObjLoader(const std::string& name, OpenGL::ResourceManager& mgr) :
mgr(mgr),
loadedPositions(3, 0), loadedTextures(2, 0), loadedNormals(3, 0),
positionCount(0), textureCount(0), normalCount(0)
{
    if (!PhysFS::exists(name))
        throw std::runtime_error("Can't find the file " + name);
    using std::cout;
    using std::endl;
    
    { // Woo, RIIA!
        // http://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
        PhysFS::FileStream pfile(name, PhysFS::OM_READ);
        file << pfile.rdbuf();
        pfile.close();
    }
    /*
    cout << "g: " << file.tellg() << " p: " << file.tellp() << endl;
    
    cout << "dump: " << endl << file.str() << endl;
    
    cout << "g: " << file.tellg() << " p: " << file.tellp() << endl;
    */
    
    file.seekg(0);
    file.seekp(0);
    
    //cout << "g: " << file.tellg() << " p: " << file.tellp() << endl;
    
    file.exceptions(std::ios::badbit | std::ios::eofbit);
    
    std::string last = "";
    std::string cmd;
    try {
        do {
            cmd = GetCommand();
            if (cmd == "g") {
                // TODO: groups are not implemented yet!
            } else if (cmd == "v") {
                ReadPosition();
            } else if (cmd == "vn") {
                ReadNormal();
            } else if (cmd == "f") {
                ReadFace();
            } else {
                // ain doin shit yet
                /*
                if (last == "") cout << '\'';
                else if (cmd != last) cout << '\'' << endl << '\'';
                cout << cmd;
                last = cmd;
                // */
            }
            nextline();
        } while (!file.eof());
    } catch (std::ios_base::failure& e) {
        // TODO: Do whatever the fuck needs to be done here!
    } catch (std::exception& e) {
        std::cerr << "Something's just thrown an exception and I'm not sure what . . . " << e.what() << endl;
    }
    if (last != "")
        cout << '\'' << endl;
    cout << "got " << positionCount << " vertexes and " << normalCount << " normals with " << trianglesCount  << " faces using them!" << endl;
    /*
    cout << "pos ind:" << endl;
    dumpvector(positionIndicies);
    cout << "norm ind:" << endl;
    dumpvector(normalIndicies);
    // */
    objects.push_back(MakeData());
}

std::string ObjLoader::GetCommand() {
    char in;
    std::ostringstream str;
    bool debugger = false;
    do {
        file.get(in);
        if (debugger) {
            std::cout << "in: '" << in << "'" << std::endl;
            size_t g = file.tellg(), p = file.tellp();
            std::cout << "g: " << g << " p: " << p << std::endl;
            std::cout << "remaining file: " << file.str() << std::endl;
            file.seekg(g);
            file.seekp(p);
            debugger = false;
        }
        if (in == '#') {
            nextline();
            continue;
        } else if (whitespace(in) || newline(in))
            // ignore initial whitespace.
            continue;
        // start reading today's command
        while(!(whitespace(in) || newline(in))) {
            str << in;
            file.get(in);
        }
        file.unget();
        break;
    } while (true);
    return str.str();
}

void ObjLoader::ReadPosition() {
    ++positionCount;
    float f;
    for (int i = 0; i < 3; ++i) {
        file >> f;
        loadedPositions.push_back(f);
    }
}

void ObjLoader::ReadNormal() {
    ++normalCount;
    float x, y, z;
    file >> x;
    file >> y;
    file >> z;
    loadedNormals.push_back(x);
    loadedNormals.push_back(y);
    loadedNormals.push_back(z);
    normalsReverseLookup[normalkey(x, y, z)] = normalCount;
}

void ObjLoader::ReadFace() {
    ++trianglesCount;
    
    index_t poses[3] = {0};
    index_t texes[3] = {0};
    index_t norms[3] = {0};

    FaceMode slashes = { 0 };
    VertexData arrs = {
        poses,
        texes,
        norms
    };
    
    // First vertex
    poses[0] = readindex(positionCount);
    char in = file.peek();
    if (in != '/') {
        slashes.PositionsOnly = true;
    } else {
        file.ignore(1);
        in = file.peek();
        if (in == '/')
            slashes.NoTextures = true;
        else
            texes[0] = readindex(textureCount);
        // If the next char is not a / then the file is technically broken.
        // Unfortunately, sponza_basic.obj does not conform to this view.
        in = file.peek();
        if (in == '/')
            file.ignore(1);
        else
            slashes.NoNormalSlash = true;
        
        in = file.peek();
        if (whitespace(in))
            slashes.NoNormals = true;
        else
            norms[0] = readindex(normalCount);
    }
    // Second Vertex
    ReadVertex(slashes, arrs, 1);
    
    // Read an arbitrary number of points
    while (ReadVertex(slashes, arrs, 2)) {
        // Debuggery
        //std::cout << "Creating a triangle of position indicies " << poses[0] << ", " << poses[1] << ", "<< poses[2] << "!" << std::endl;
        // grab the face normals in leu of vertex normals
        if (slashes.PositionsOnly || slashes.NoNormals) {
            //std::cout << "Deducing normals for " << poses[0] << ", " << poses[1] << ", "<< poses[2] << ": ";
            glm::vec3 a,b,c;
            try {
                a = getvec(loadedPositions, poses[0]),
                b = getvec(loadedPositions, poses[1]),
                c = getvec(loadedPositions, poses[2]);
            } catch (std::exception& e) {
                std::cerr << "Attempted to access a non existent vertex! Trying them manually now..." << std::endl;
                std::cerr << poses[0] << "...";
                getvec(loadedPositions, poses[0]);
                std::cerr << poses[1] << "...";
                getvec(loadedPositions, poses[1]);
                std::cerr << poses[2] << "...";
                getvec(loadedPositions, poses[2]);
                std::cerr << "Done!";
            }
            glm::vec3 norm = glm::normalize(glm::cross(c - a, b - a));
            //std::cout << "(" << norm.x << "," << norm.y << "," << norm.z << "). ";
            auto key = normalkey(norm);
            const auto existing = normalsReverseLookup.find(key);
            index_t ind;
            if (existing != normalsReverseLookup.end()) {
                ind = existing->second;
                //std::cout << "This is the existing normal with index: ";
            } else {
                normalCount++;
                ind = normalCount;
                loadedNormals.push_back(norm.x);
                loadedNormals.push_back(norm.y);
                loadedNormals.push_back(norm.z);
                normalsReverseLookup[key] = normalCount;
                //std::cout << "This is a new normal, now assinged index: ";
            }
            //std::cout << ind << std::endl;
            norms[0] = ind;
            norms[1] = ind;
            norms[2] = ind;
        }
        // Throw the indicies into the global pot
        for (auto p : poses)
            positionIndicies.push_back(p);
        for (auto t : texes)
            textureIndicies.push_back(t);
        for (auto n : norms)
            normalIndicies.push_back(n);
        // Shift the vertex back one for the triangle fan
        poses[1] = poses[2];
        texes[1] = texes[2];
        norms[1] = norms[2];
    }
}

bool ObjLoader::ReadVertex(const FaceMode& mode, const VertexData& verts, const size_t index) {
    if (file.fail()) {
        std::cout << "wtf?";
    }
    while(true) {
        char c;
        file.get(c);
        if (whitespace(c))
            continue;
        file.unget();
        if (file.fail()) {
            std::cout << "wtf?";
        }
        if (newline(c))
            return false;
        break;
    }

    verts.positions[index] = readindex(positionCount);
    if (mode.PositionsOnly)
        return true;
    file.ignore(1);
    if (!mode.NoTextures)
        verts.textures[index] = readindex(textureCount);
    if (!mode.NoNormalSlash)
        file.ignore(1);
    if (!mode.NoNormals)
        verts.normals[index] = readindex(normalCount);
    return true;
}

RenderData ObjLoader::MakeData() {

    std::vector<float> positions(3, 0);
  //std::vector<float> texures(2, 0);
    std::vector<float> normals(3, 0);
    positions.reserve(loadedPositions.size());
  //textures .reserve(loadedTextures.size());
    normals  .reserve(loadedNormals.size());
    std::vector<index_t> indicies;
    std::map<indexid_t, index_t> indmap;
    index_t numinds = 0;
    auto pos = positionIndicies.begin(), norm = normalIndicies.begin(), end = positionIndicies.end();
    for (; pos != end; ++pos, ++norm) {
        //std::cout << "pos: " << *pos << " norm: " << *norm;
        auto ind = compileindex(*pos, 0, *norm);
        auto loc = indmap.find(ind);
        if (loc != indmap.end()) {
            //std::cout << " found: " << loc->second;
            indicies.push_back(loc->second);
        } else {
            ++numinds;
            addvars(positions, loadedPositions, *pos);
          //addvars(textures,  loadedTextures, *tex);
            addvars(normals,   loadedNormals, *norm);
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
    
    
    auto vbos = mgr.CreateVBOs(2);
    GLuint VAB = vbos[0];
    GLuint EAB = vbos[1];
    GLuint VAO = mgr.CreateVAO();
    
    RenderData ret = {
        EAB,
        VAO,
        0,
        GL_UNSIGNED_INT
    };
    
    /*
     * Vertacies
     */
    GLsizei numv  = positions.size();
  //GLsizei numt  = textures .size();
    GLsizei numvn = normals  .size();
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
    
    ret.IndexCount = numi;
    return ret;
}

inline
void ObjLoader::nextline() {
    const auto everything = std::numeric_limits<std::streamsize>::max();
    file.ignore(everything, '\n');
}

inline
bool whitespace(const char c) {
    return c == ' ' || c == '\t';
}
inline
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

ObjLoader::index_t ObjLoader::readindex(size_t existing_count) {
    rawindex_t s;
    file >> s;
    if (s < 0) 
        s += existing_count + 1;
    return s;
}

void ObjLoader::addvars(std::vector<float>& dest, const std::vector<float>& src, size_t start, size_t num) const {
    start *= num;
    for (int i = 0; i < num; ++i)
        dest.push_back(src.at(start + i));
}

inline
glm::vec3 ObjLoader::getvec(const std::vector<float>& src, size_t start) const {
    start *= 3;
    return glm::vec3(src.at(start), src.at(start + 1), src.at(start + 2));
}

inline
ObjLoader::indexid_t ObjLoader::compileindex(index_t pos, index_t tex, index_t norm) const {
    return std::make_tuple(pos, tex, norm);
}

inline
ObjLoader::normalkey_t ObjLoader::normalkey(float x, float y, float z) const {
    return std::make_tuple(x, y, z);
}

inline
ObjLoader::normalkey_t ObjLoader::normalkey(const glm::vec3& vec) const {
    return std::make_tuple(vec.x, vec.y, vec.z);
}










