#ifndef GL_GL_HPP
#define GL_GL_HPP

#ifdef __APPLE__

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#elif __linux__

#include <GL/freeglut.h>
#include <GL/glew.h>

#else

#endif

namespace gl {

void init();

} // namespace gl

#endif // GL_GL_HPP