#include <common/program.hpp>

namespace {

GLuint create_vao() {
  GLuint VAO = 0;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  return VAO;
}

} // namespace

gl::program_t::program_t(const shader_t &shader) : shader(shader) {}

common::result_t<GLuint> gl::program_t::create() {
  program_id = glCreateProgram();
  if (program_id == 0) {
    constexpr auto err = "error creating shader program";
    LOG_ERR(err);
    return {0, err};
  }

  return {program_id, std::nullopt};
}

common::result_t<> gl::program_t::compile() {
  auto res = shader.compile();
  if (res.err != std::nullopt) {
    LOG_ERR(res.err.value());
    return {common::none_v, res.err};
  }

  shader.attach_to_program(program_id);

  return {common::none_v, std::nullopt};
}

common::result_t<> gl::program_t::link() {
  glLinkProgram(program_id);

  GLint success = 0;
  GLchar err_log[1024] = {'\0'};
  glGetProgramiv(program_id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program_id, sizeof(err_log), NULL, err_log);
    auto err = common::make_str("Error linking shader program: ", err_log);
    LOG_ERR(err);
    return {common::none_v, err};
  }

  return {common::none_v, std::nullopt};
}

common::result_t<> gl::program_t::validate() {
  glValidateProgram(program_id);

  GLint success = 0;
  GLchar err_log[1024] = {'\0'};
  glGetProgramiv(program_id, GL_VALIDATE_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program_id, sizeof(err_log), NULL, err_log);
    auto err = common::make_str("Invalid shader program: ", err_log);
    LOG_ERR(err);
    return {common::none_v, err};
  }

  return {common::none_v, std::nullopt};
}

common::result_t<GLuint> gl::program_t::build() {
  vao_id = create_vao();

  auto res_0 = create();
  if (res_0.err != std::nullopt) {
    LOG_ERR(res_0.err.value());
    return {0, res_0.err};
  }

  auto res_1 = compile();
  if (res_1.err != std::nullopt) {
    LOG_ERR(res_1.err.value());
    return {0, res_1.err};
  }

  res_1 = link();
  if (res_1.err != std::nullopt) {
    LOG_ERR(res_1.err.value());
    return {0, res_1.err};
  }

  res_1 = validate();
  if (res_1.err != std::nullopt) {
    LOG_ERR(res_1.err.value());
    return {0, res_1.err};
  }

  return {program_id, std::nullopt};
}

void gl::program_t::use() { glUseProgram(program_id); }