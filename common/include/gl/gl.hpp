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

inline constexpr glm::vec3 white(1.0f);
inline constexpr glm::vec3 black(0.0f);
inline constexpr glm::vec3 gray(0.5f);
inline constexpr glm::vec3 red = {1.0f, 0.0f, 0.0f};
inline constexpr glm::vec3 green = {0.0f, 1.0f, 0.0f};
inline constexpr glm::vec3 blue = {0.0f, 0.0f, 1.0f};

} // namespace gl

#endif // GL_GL_HPP