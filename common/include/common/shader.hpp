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

class shader_t final {
public:
  shader_t(const std::string &vert_glsl, const std::string &frag_glsl,
           bool is_file);

  common::result_t<GLuint> build();
  common::result_t<> use() const;

  const shader_id_t &shader_id() const;
  GLuint program_id() const;
  GLuint vao() const;

private:
  using compile_func_t = common::result_t<shader_id_t>();

  common::result_t<GLuint> create();
  common::result_t<shader_id_t> compile();
  common::result_t<> link();
  common::result_t<> validate() const;
  void attach_to_program();

  std::function<compile_func_t> _compile = nullptr;
  shader_id_t _shader_id = {0, 0};
  GLuint _program_id = 0;
};

} // namespace gl

#endif // COMMON_SHADER_HPP