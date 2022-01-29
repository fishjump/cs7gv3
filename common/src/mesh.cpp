#include <common/mesh.hpp>

gl::mesh_t::mesh_t(const shader_t &shader) : shader(shader) {}

common::result_t<> gl::mesh_t::draw() const {
  glBindVertexArray(_vao);

  auto res = shader.use();
  if (res.err != std::nullopt) {
    LOG_ERR(res.err.value());
    return {common::none_v, res.err};
  }
  glDrawArrays(GL_TRIANGLES, 0, 3);

  glBindVertexArray(NULL);

  return {common::none_v, std::nullopt};
}