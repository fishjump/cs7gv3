#ifndef SHADER_H
#define SHADER_H

#include <memory>
#include <vector>

#include <common.hpp>
#include <opengl.hpp>

namespace shader {

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

} // namespace shader

#endif // SHADER_H