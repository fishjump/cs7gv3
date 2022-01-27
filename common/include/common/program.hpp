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

struct program_t {
  program_t(const shader_t &shader);

  common::result_t<GLuint> create();
  common::result_t<> compile();
  common::result_t<> link();
  common::result_t<> validate();

  common::result_t<GLuint> build();

  template <class T> void create_vbo(const T &t);
  template <class T>
  void use_vbo(const T &t, const std::vector<std::string> &attributes);

  void use();

private:
  shader_t shader;
  GLuint program_id;
  GLuint vao_id;
};

} // namespace gl

template <class T> void gl::program_t::create_vbo(const T &t) {
  GLuint VBO;
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(T), NULL, GL_STATIC_DRAW);

  boost::pfr::for_each_field(t, [&t](const auto &field) {
    glBufferSubData(GL_ARRAY_BUFFER, common::offset_diff(field, t),
                    sizeof(field), field.data());
  });
}

template <class T>
void gl::program_t::use_vbo(const T &t,
                                const std::vector<std::string> &attributes) {
  boost::pfr::for_each_field(t, [&t, &attributes, this](const auto &field,
                                                        int index) {
    GLuint id =
        glGetAttribLocation(this->program_id, attributes[index].c_str());
    glEnableVertexAttribArray(id);
    glVertexAttribPointer(
        id, sizeof(common::value_type_t<decltype(field)>) / sizeof(GL_FLOAT),
        GL_FLOAT, GL_FALSE, 0, (void *)common::offset_diff(field, t));
  });
}

#endif // COMMON_PROGRAM_HPP