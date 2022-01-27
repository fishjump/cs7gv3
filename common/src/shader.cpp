#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include <common/opengl.hpp>
#include <common/shader.hpp>

#define EXT_LIST                                                               \
  EXT("vert", GL_VERTEX_SHADER)                                                \
  EXT("frag", GL_FRAGMENT_SHADER)

/// internal func declare
namespace {

#define EXT(x, y) {x, y},
std::unordered_map<std::string, GLenum> ext_to_shader_type_map = {EXT_LIST};
#undef EXT

#define EXT(x, y) #x ", "
constexpr auto expect_exts = EXT_LIST "\b\b  ";
#undef EXT

inline common::result_t<std::shared_ptr<std::string>>
load_shader(const std::string &file);

inline common::result_t<GLuint> compile_shader(const std::string &glsl_file);
common::result_t<GLuint> compile_shader(const char *glsl_code,
                                        const GLenum shader_type);
inline common::result_t<GLuint> compile_shader(const std::string &glsl_code,
                                               const GLenum shader_type);
inline common::result_t<std::vector<GLuint>>
compile_shaders(const std::vector<std::string> &glsl_files);

inline common::result_t<GLuint>
create_program(const std::vector<std::string> &glsl_files);

} // namespace

gl::shader_t::shader_t(const std::string &vert_glsl,
                           const std::string &frag_glsl, bool is_file) {
  std::function compile = [](const std::string &glsl, const GLenum shader_type)
      -> common::result_t<GLuint> { return compile_shader(glsl, shader_type); };

  if (is_file) {
    compile = [](const std::string &glsl,
                 const GLenum shader_type) -> common::result_t<GLuint> {
      return compile_shader(glsl);
    };
  }

  _compile = [vert_glsl, frag_glsl, compile]() -> compile_ret_t {
    auto res = compile(vert_glsl, GL_VERTEX_SHADER);
    if (res.err != std::nullopt) {
      LOG_ERR(res.err.value());
      return {{}, res.err};
    }
    GLuint vert_id = res.result;

    res = compile(frag_glsl, GL_FRAGMENT_SHADER);
    if (res.err != std::nullopt) {
      LOG_ERR(res.err.value());
      return {{}, res.err};
    }
    GLuint frag_id = res.result;

    return {{vert_id, frag_id}, std::nullopt};
  };
}

gl::shader_t::compile_ret_t gl::shader_t::compile() {
  if (_compile == nullptr) {
    constexpr auto err = "null _compile func ptr";
    LOG_ERR(err);
    return {{}, err};
  }

  auto res = _compile();
  if (res.err != std::nullopt) {
    LOG_ERR(res.err.value());
    return res;
  }
  id = res.result;

  return res;
}

void gl::shader_t::attach_to_program(GLuint program_id) {
  glAttachShader(program_id, id.frag_id);
  glAttachShader(program_id, id.vert_id);
}

/// internal func impl
namespace {

inline common::result_t<GLenum> get_shader_type(const std::string &ext) {
  auto iter = ext_to_shader_type_map.find(ext);
  if (iter == ext_to_shader_type_map.end()) {
    return {0, common::make_str("malformed extension name: ", ext,
                                ", expect: ", expect_exts)};
  }

  return {iter->second, std::nullopt};
}

inline common::result_t<std::shared_ptr<std::string>>
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

inline common::result_t<GLuint> compile_shader(const std::string &glsl_file) {
  auto res_0 = common::get_ext(glsl_file);
  if (res_0.err != std::nullopt) {
    return {0, res_0.err};
  }

  auto res_1 = get_shader_type(res_0.result);
  if (res_1.err != std::nullopt) {
    return {0, res_1.err};
  }

  auto res_2 = load_shader(glsl_file);
  if (res_2.err != std::nullopt) {
    return {0, res_2.err};
  }

  return compile_shader(res_2.result->c_str(), res_1.result);
}

inline common::result_t<GLuint> compile_shader(const char *glsl_code,
                                               const GLenum shader_type) {
  GLuint shader_obj = glCreateShader(shader_type);
  if (shader_obj == 0) {
    return {0, common::make_str("error creating shader type ", shader_type)};
  }

  glShaderSource(shader_obj, 1, (const GLchar **)&glsl_code, NULL);
  glCompileShader(shader_obj);

  GLint success;
  glGetShaderiv(shader_obj, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar log[1024];
    glGetShaderInfoLog(shader_obj, 1024, NULL, log);
    return {0, common::make_str("Error compiling shader type ", shader_type,
                                ": ", log)};
  }

  return {shader_obj, std::nullopt};
}

inline common::result_t<GLuint> compile_shader(const std::string &glsl_code,
                                               const GLenum shader_type) {
  const char *glsl_cstr = glsl_code.c_str();
  return compile_shader(glsl_cstr, shader_type);
}

} // namespace
