#ifndef COMMON_OPENGL_H
#define COMMON_OPENGL_H

#ifdef __APPLE__

#include <GL/glew.h>
#include <GLUT/glut.h>
#include <OpenGL/gl3.h>

#elif __linux__

#include <GL/freeglut.h>
#include <GL/glew.h>

#else

#endif

#include <common/common.hpp>

namespace gl {

struct vec3_t {
  GLfloat _1;
  GLfloat _2;
  GLfloat _3;
};

struct vec4_t {
  GLfloat _1;
  GLfloat _2;
  GLfloat _3;
  GLfloat _4;
};

struct win_size_t {
  int height;
  int width;
};

struct init_config_t {
  std::string mw_title;
  win_size_t win_size;
  uint32_t display_mode;
  int argc;
  char **argv;
};

common::result_t<> init(init_config_t &cfg);

} // namespace gl

#endif // COMMON_OPENGL_H