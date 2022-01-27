#ifndef COMMON_SHADER_HPP
#define COMMON_SHADER_HPP

#include <array>
#include <memory>
#include <vector>

#include <common/common.hpp>

namespace gl {

struct shader_id_t final {
  GLuint vert_id;
  GLuint frag_id;
};

struct shader_t final {
  using compile_ret_t = common::result_t<shader_id_t>;
  using compile_func_t = compile_ret_t();

  shader_t(const std::string &vert_glsl, const std::string &frag_glsl,
           bool is_file);

  compile_ret_t compile();
  void attach_to_program(GLuint program_id);

private:
  std::function<compile_func_t> _compile = nullptr;
  shader_id_t id = {0, 0};
};

} // namespace gl

#endif // COMMON_SHADER_HPP