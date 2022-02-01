#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include <gl/gl.hpp>
#include <gl/shader.hpp>

// internal func declare
namespace {

common::result_t<std::shared_ptr<std::string>>
load_shader(const std::string &file);

common::result_t<GLuint> compile_shader(const std::string &glsl_file,
                                        const GLenum shader_type);

} // namespace

const gl::shader_id_t &gl::shader_t::shader_id() const { return _shader_id; }
GLuint gl::shader_t::program_id() const { return _program_id; }

gl::shader_t::shader_t(const std::string &vert_glsl,
                       const std::string &frag_glsl) {
  if (auto res = build(vert_glsl, frag_glsl); res.err != std::nullopt) {
    LOG_ERR(res.err.value());
    exit(1);
  }
}

common::result_t<GLuint> gl::shader_t::build(const std::string &vert_glsl,
                                             const std::string &frag_glsl) {
  if (auto res = create(); res.err != std::nullopt) {
    LOG_ERR(res.err.value());
    return {0, res.err};
  }

  if (auto res = compile(vert_glsl, frag_glsl); res.err != std::nullopt) {
    LOG_ERR(res.err.value());
    return {0, res.err};
  }

  if (auto res = link(); res.err != std::nullopt) {
    LOG_ERR(res.err.value());
    return {0, res.err};
  }

  return {_program_id, std::nullopt};
}

void gl::shader_t::use() const { glUseProgram(_program_id); }

common::result_t<GLuint> gl::shader_t::create() {
  _program_id = glCreateProgram();
  if (_program_id == 0) {
    constexpr auto err = "error creating shader program";
    LOG_ERR(err);
    return {0, err};
  }

  return {_program_id, std::nullopt};
}

common::result_t<gl::shader_id_t>
gl::shader_t::compile(const std::string &vert_glsl,
                      const std::string &frag_glsl) {
  shader_id_t ids = {0, 0};

  auto res = compile_shader(vert_glsl, GL_VERTEX_SHADER);
  if (res.err != std::nullopt) {
    LOG_ERR(res.err.value());
    return {{0, 0}, res.err};
  }
  ids.vert_id = res.result;

  res = compile_shader(frag_glsl, GL_FRAGMENT_SHADER);
  if (res.err != std::nullopt) {
    LOG_ERR(res.err.value());
    return {{0, 0}, res.err};
  }
  ids.frag_id = res.result;

  _shader_id = ids;

  attach_to_program();

  return {_shader_id, std::nullopt};
}

common::result_t<> gl::shader_t::link() {
  glLinkProgram(_program_id);

  GLint success = 0;
  glGetProgramiv(_program_id, GL_LINK_STATUS, &success);
  if (!success) {
    GLchar err_log[1024] = {'\0'};
    glGetProgramInfoLog(_program_id, sizeof(err_log), NULL, err_log);
    auto err = common::make_str("Error linking shader program: ", err_log);
    LOG_ERR(err);
    return {common::none_v, err};
  }

  return {common::none_v, std::nullopt};
}

common::result_t<> gl::shader_t::validate() const {
  glValidateProgram(_program_id);

  GLint success = 0;
  GLchar err_log[1024] = {'\0'};
  glGetProgramiv(_program_id, GL_VALIDATE_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(_program_id, sizeof(err_log), NULL, err_log);
    auto err = common::make_str("Invalid shader program: ", err_log);
    LOG_ERR(err);
    return {common::none_v, err};
  }

  return {common::none_v, std::nullopt};
}

void gl::shader_t::attach_to_program() {
  glAttachShader(_program_id, _shader_id.vert_id);
  glAttachShader(_program_id, _shader_id.frag_id);
}

// internal func impl
namespace {

common::result_t<std::shared_ptr<std::string>>
load_shader(const std::string &file) {
  std::ifstream fs;

  fs.open(file);
  if (!fs.is_open()) {
    return {nullptr, common::make_str("fail to open file: ", file)};
  }
  defer(fs.close());

  std::stringstream ss;
  while (!fs.eof()) {
    std::string line;
    std::getline(fs, line);
    ss << line << "\n";
  }

  return {std::make_shared<std::string>(ss.str()), std::nullopt};
}

common::result_t<GLuint> compile_shader(const std::string &glsl_file,
                                        const GLenum shader_type) {
  auto res = load_shader(glsl_file);
  if (res.err != std::nullopt) {
    LOG_ERR(res.err.value());
    return {0, res.err};
  }

  GLuint shader_id = glCreateShader(shader_type);
  if (shader_id == 0) {
    const auto err = common::make_str(
        "filename: ", glsl_file, ", error creating shader type ", shader_type);
    LOG_ERR(err);
    return {0, err};
  }
  const GLchar *glsl_code = res.result->c_str();

  glShaderSource(shader_id, 1, (const GLchar **)&glsl_code, NULL);
  glCompileShader(shader_id);

  GLint success;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar log[1024];
    glGetShaderInfoLog(shader_id, 1024, NULL, log);
    const auto err = common::make_str("filename: ", glsl_file,
                                      ", error compiling shader type ",
                                      shader_type, ":", log);
    return {0, err};
  }

  return {shader_id, std::nullopt};
}

} // namespace
