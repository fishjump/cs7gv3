#ifndef SHADER_H
#define SHADER_H

#include <memory>
#include <vector>

#include <common.hpp>
#include <opengl.hpp>

namespace shader {

struct vbo_t {
  opengl::vec3_t vertices[3];
  opengl::vec4_t colors[3];
};

bool support_shader_binary();

common::result_t<std::shared_ptr<std::string>>
load_shader(const std::string &file);

common::result_t<GLuint> compile_shader(const std::string &glsl_file);
common::result_t<GLuint> compile_shader(const char *glsl_code,
                                        const GLenum shader_type);
common::result_t<GLuint> compile_shader(const std::string &glsl_code,
                                        const GLenum shader_type);
common::result_t<std::vector<GLuint>>
compile_shaders(const std::vector<std::string> &glsl_files);

common::result_t<GLuint>
create_program(const std::vector<std::string> &glsl_files);

GLuint vbo_gen(vbo_t vbo);
void link_buf(GLuint program_id);

} // namespace shader

#endif // SHADER_H