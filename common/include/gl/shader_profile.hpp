#ifndef COMMON_GL_SHADER_PROFILE_HPP
#define COMMON_GL_SHADER_PROFILE_HPP

#include <unordered_map>

#include <common.hpp>

#include "gl.hpp"

namespace gl {

struct meta_profile_t {
  std::unordered_map<std::string, GLboolean> booleans;
  std::unordered_map<std::string, GLint> integrals;
  std::unordered_map<std::string, GLfloat> floats;
  std::unordered_map<std::string, glm::vec2> vec2s;
  std::unordered_map<std::string, glm::vec3> vec3s;
  std::unordered_map<std::string, glm::vec4> vec4s;
  std::unordered_map<std::string, glm::mat2> mat2s;
  std::unordered_map<std::string, glm::mat3> mat3s;
  std::unordered_map<std::string, glm::mat4> mat4s;
};

class shader_profile_t {
public:
  virtual const meta_profile_t &meta();

  material_t material;
  light_t light;

  glm::vec3 view_pos;

protected:
  meta_profile_t _meta;
};

class phong_profile_t final : public shader_profile_t {
public:
  const meta_profile_t &meta();
};

class gooch_profile_t final: public shader_profile_t {
public:
  const meta_profile_t &meta();

  GLfloat a = 0.2;
  GLfloat b = 0.6;
  glm::vec3 k_blue = {0.0f, 0.0f, 0.4f};
  glm::vec3 k_yellow = {0.4f, 0.4f, 0.0f};
};

class cook_torrance_profile_t : public shader_profile_t {
public:
  virtual const meta_profile_t &meta();
};

template <class T>
void insert(meta_profile_t &meta, const std::string &key, const T &value);

} // namespace gl

template <class T>
void gl::insert(meta_profile_t &meta, const std::string &key, const T &value) {
  if constexpr (std::is_same_v<T, GLboolean>) {
    meta.booleans.insert(std::make_pair(key, value));
  } else if constexpr (std::is_integral_v<T>) {
    meta.integrals.insert(std::make_pair(key, value));
  } else if constexpr (std::is_same_v<T, GLfloat>) {
    meta.floats.insert(std::make_pair(key, value));
  } else if constexpr (common::is_glm_vec_v<T, 2>) {
    meta.vec2s.insert(std::make_pair(key, value));
  } else if constexpr (common::is_glm_vec_v<T, 3>) {
    meta.vec3s.insert(std::make_pair(key, value));
  } else if constexpr (common::is_glm_vec_v<T, 4>) {
    meta.vec4s.insert(std::make_pair(key, value));
  } else if constexpr (common::is_glm_mat_v<T, 2>) {
    meta.mat2s.insert(std::make_pair(key, value));
  } else if constexpr (common::is_glm_mat_v<T, 3>) {
    meta.mat3s.insert(std::make_pair(key, value));
  } else if constexpr (common::is_glm_mat_v<T, 4>) {
    meta.mat4s.insert(std::make_pair(key, value));
  } else {
    static_assert(common::dependent_false_v<T>, "unsupported type");
  }
}

#endif // COMMON_GL_SHADER_PROFILE_HPP