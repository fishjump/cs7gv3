#include <common.hpp>
#include <opengl.hpp>

namespace {

void init_glut(opengl::init_config_t &cfg) {
  glutInit(&cfg.argc, cfg.argv);
  glutInitDisplayMode(cfg.display_mode);
  glutInitWindowSize(cfg.win_size.height, cfg.win_size.width);
  glutCreateWindow(cfg.mw_title.c_str());
  glutDisplayFunc([]() {
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glutSwapBuffers();
  });
}

common::result_t<common::none_t> init_glew() {
  GLenum res = glewInit();
  if (res != GLEW_OK) {
    std::string err = (const char *)glewGetErrorString(res);
    LOG_ERR(err);
    return {common::none_v, err};
  }

  return {common::none_v, std::nullopt};
}

} // namespace

common::result_t<common::none_t> opengl::init(init_config_t &cfg) {
  init_glut(cfg);
  auto res = init_glew();
  if (res.err != std::nullopt) {
    LOG_ERR(res.err.value());
    return res;
  }

  return {common::none_v, std::nullopt};
}