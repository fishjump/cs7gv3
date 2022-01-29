#ifndef COMMON_PROGRAM_HPP
#define COMMON_PROGRAM_HPP

#include <array>
#include <memory>
#include <vector>

#include <boost/pfr.hpp>

#include <common/common.hpp>
#include <common/opengl.hpp>
#include <common/shader.hpp>

namespace gl {

struct mesh_t {
  mesh_t(const shader_t &shader);

  template <class T> void use_vbo(const T &t);

  common::result_t<> draw() const;

private:
  const shader_t &shader;
  GLuint _vao = 0;
};

} // namespace gl

template <class T> void gl::mesh_t::use_vbo(const T &t) {
  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);

  GLuint VBO = 0;
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(T), NULL, GL_STATIC_DRAW);

  boost::pfr::for_each_field(t, [&t](const auto &field) {
    glBufferSubData(GL_ARRAY_BUFFER, common::offset_diff(field, t),
                    sizeof(field), field.data());
  });

  boost::pfr::for_each_field(t, [&t](const auto &field, int index) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(
        index, sizeof(common::value_type_t<decltype(field)>) / sizeof(GL_FLOAT),
        GL_FLOAT, GL_FALSE, 0, (void *)common::offset_diff(field, t));
  });
}

#endif // COMMON_PROGRAM_HPP