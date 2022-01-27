#ifndef CS7GV3_PROGRAM_HPP
#define CS7GV3_PROGRAM_HPP

#include <array>
#include <memory>
#include <vector>

#include <common.hpp>
#include <opengl.hpp>
#include <shader.hpp>

namespace cs7gv3 {

struct program_t {
  program_t(const shader_t &shader);

  common::result_t<GLuint> create();
  common::result_t<> compile();
  common::result_t<> link();
  common::result_t<> validate();

  common::result_t<GLuint> build();

  common::result_t<> create_vbo(opengl::triangle_t triangle);
  common::result_t<> use_vbo(opengl::triangle_t triangle);

  void use();

private:
  shader_t shader;
  GLuint program_id;
  GLuint vao_id;
};

} // namespace cs7gv3

#endif // CS7GV3_PROGRAM_HPP