#include <gl.hpp>

gl::mesh_t::mesh_t(const std::vector<vertex_t> &vertices,
                   const std::vector<uint32_t> &indices,
                   const std::vector<texture_t> &textures)
    : _vertices(vertices), _indices(indices), _textures(textures) {
  init();
}

void gl::mesh_t::init() {
  glGenVertexArrays(1, &_vao);
  glGenBuffers(1, &_vbo);
  glGenBuffers(1, &_ebo);

  glBindVertexArray(_vao);
  defer(glBindVertexArray(NULL));

  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER,
               _vertices.size() *
                   sizeof(common::value_type_t<decltype(_vertices)>),
               _vertices.cbegin().base(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               _indices.size() *
                   sizeof(common::value_type_t<decltype(_indices)>),
               _indices.cbegin().base(), GL_STATIC_DRAW);

  const auto b = *_vertices.cbegin();
  boost::pfr::for_each_field(b, [&b](const auto &field, int index) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, sizeof(field) / sizeof(GL_FLOAT), GL_FLOAT,
                          GL_FALSE, sizeof(b),
                          (void *)common::offset_diff(field, b));
  });
}

common::result_t<> gl::mesh_t::draw(const shader_t &shader) const {
  glBindVertexArray(_vao);
  defer(glBindVertexArray(NULL));

  auto res = shader.use();
  if (res.err != std::nullopt) {
    LOG_ERR(res.err.value());
    return {common::none_v, res.err};
  }
  glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);

  return {common::none_v, std::nullopt};
}