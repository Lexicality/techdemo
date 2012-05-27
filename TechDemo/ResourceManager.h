#pragma once
#include <vector>
#include <stdexcept>
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif //__APPLE__

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
