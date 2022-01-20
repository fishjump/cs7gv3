#ifndef OPENGL_H
#define OPENGL_H

#ifdef __APPLE__

#include <GL/glew.h>
#include <GLUT/glut.h>
#include <OpenGL/gl3.h>

#elif __linux__

#include <GL/freeglut.h>
#include <GL/glew.h>

#else

#endif

namespace opengl {

struct w_size_t {
  int height;
  int width;
};

struct init_config_t {
  std::string mw_title;
  w_size_t w_size;
  uint32_t display_mode;
  int argc;
  char **argv;
};

common::result_t<common::none_t> init(init_config_t &cfg);

} // namespace opengl

#endif // OPENGL_H