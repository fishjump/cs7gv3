#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include <common.hpp>
#include <shader.hpp>

#define EXT_LIST                                                               \
  EXT("vert", GL_VERTEX_SHADER)                                                \
  EXT("frag", GL_FRAGMENT_SHADER)

namespace {

#define EXT(x, y) {x, y},
std::unordered_map<std::string, GLenum> ext_to_shader_type_map = {EXT_LIST};
#undef EXT

#define EXT(x, y) #x ", "
const char *expect_exts = EXT_LIST "\b\b  ";
#undef EXT

inline common::result_t<GLenum> get_shader_type(const std::string &ext) {
  auto iter = ext_to_shader_type_map.find(ext);
  if (iter == ext_to_shader_type_map.end()) {
    return {0, common::make_str("malformed extension name: ", ext,
                                ", expect: ", expect_exts)};
  }

  return {iter->second, std::nullopt};
}

inline void attach_shaders(GLuint program_id,
                           const std::vector<GLuint> &shader_ids) {
  for (const auto &id : shader_ids) {
    glAttachShader(program_id, id);
  }
}

inline common::result_t<common::none_t> link_program(GLuint program_id) {
  glLinkProgram(program_id);

  GLint success = 0;
  GLchar err_log[1024] = {'\0'};
  glGetProgramiv(program_id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program_id, sizeof(err_log), NULL, err_log);
    std::string err =
        common::make_str("Error linking shader program: ", err_log);
    LOG_ERR(err);
    return {common::none_v, err};
  }

  return {common::none_v, std::nullopt};
}

inline common::result_t<common::none_t> validate_program(GLuint program_id) {
  glValidateProgram(program_id);

  GLint success = 0;
  GLchar err_log[1024] = {'\0'};
  glGetProgramiv(program_id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program_id, sizeof(err_log), NULL, err_log);
    std::string err = common::make_str("Invalid shader program: ", err_log);
    LOG_ERR(err);
    return {common::none_v, err};
  }

  return {common::none_v, std::nullopt};
}

} // namespace

bool shader::support_shader_binary() {
  GLint formats = 0;
  glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);
  if (formats < 1) {
    std::cerr << "Driver does not support any binary formats." << std::endl;
    exit(EXIT_FAILURE);
  }

  return formats > 0;
}

common::result_t<std::shared_ptr<std::string>>
shader::load_shader(const std::string &file) {
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

common::result_t<GLuint> shader::compile_shader(const std::string &glsl_file) {
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

common::result_t<GLuint> shader::compile_shader(const char *glsl_code,
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

common::result_t<GLuint> shader::compile_shader(const std::string &glsl_code,
                                                const GLenum shader_type) {
  const char *glsl_cstr = glsl_code.c_str();
  return compile_shader(glsl_cstr, shader_type);
}

common::result_t<std::vector<GLuint>>
shader::compile_shaders(const std::vector<std::string> &glsl_files) {
  std::vector<GLuint> obj_id_vec;
  for (const auto &file : glsl_files) {
    auto res = shader::compile_shader(file);
    if (res.err != std::nullopt) {
      LOG_ERR(res.err.value());
      return {{}, res.err};
    }
    obj_id_vec.push_back(res.result);
  }

  return {obj_id_vec, std::nullopt};
}

common::result_t<GLuint>
shader::create_program(const std::vector<std::string> &glsl_files) {

  GLuint program_id = glCreateProgram();
  if (program_id == 0) {
    constexpr char *err = "error creating shader program";
    LOG_ERR(err);
    return {0, err};
  }

  auto res_0 = compile_shaders(glsl_files);
  if (res_0.err != std::nullopt) {
    LOG_ERR(res_0.err.value());
    return {0, res_0.err};
  }
  std::vector<GLuint> shader_ids = res_0.result;

  attach_shaders(program_id, shader_ids);

  constexpr size_t VAO_SIZE = 2;
  GLuint *VAO = new GLuint[VAO_SIZE];
  glGenVertexArrays(VAO_SIZE, VAO);
  for (int i = 0; i < VAO_SIZE; i++) {
    glBindVertexArray(VAO[i]);
  }

  auto res_1 = link_program(program_id);
  if (res_1.err != std::nullopt) {
    LOG_ERR(res_1.err.value());
    return {0, res_1.err};
  }

  auto res_2 = validate_program(program_id);
  if (res_2.err != std::nullopt) {
    LOG_ERR(res_2.err.value());
    return {0, res_2.err};
  }

  return {program_id, std::nullopt};
}

GLuint shader::vbo_gen(vbo_t vbo) {
  GLuint VBO;
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vbo_t), NULL, GL_STATIC_DRAW);

  glBufferSubData(GL_ARRAY_BUFFER, CLS_OFFSET_BEG(vbo_t, vertices),
                  CLS_OFFSET_LEN(vbo_t, vertices), vbo.vertices);
  glBufferSubData(GL_ARRAY_BUFFER, CLS_OFFSET_BEG(vbo_t, colors),
                  CLS_OFFSET_LEN(vbo_t, colors), vbo.colors);
  return VBO;
}

void shader::link_buf(GLuint program_id) {
  GLuint pos_id = glGetAttribLocation(program_id, "vPosition");
  GLuint color_id = glGetAttribLocation(program_id, "vColor");

  glEnableVertexAttribArray(pos_id);
  glVertexAttribPointer(pos_id, SIZE_BY(vbo_t::vertices, GLfloat), GL_FLOAT,
                        GL_FALSE, 0, (void *)(CLS_OFFSET_BEG(vbo_t, vertices)));

  glEnableVertexAttribArray(color_id);
  glVertexAttribPointer(color_id, SIZE_BY(vbo_t::colors, GLfloat), GL_FLOAT,
                        GL_FALSE, 0, (void *)CLS_OFFSET_BEG(vbo_t, colors));
}
