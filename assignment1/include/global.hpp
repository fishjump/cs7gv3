#pragma once

#include <cs7gvx_utils.hpp>

#include "teapot_model.hpp"

namespace {

cs7gvx_utils::gl::light_t light = {
    .position = {0.0f, 2.0f, 8.0f},
    .diffuse_color = cs7gvx_utils::gl::white * glm::vec3(0.5f),
    .ambient_color = cs7gvx_utils::gl::white,
    .specular_color = glm::vec3(1.0f),
};

cs7gvx_utils::gl::material_t material = {
    .shininess = 16,
    .ambient_color = cs7gvx_utils::gl::gray,
    .diffuse_color = cs7gvx_utils::gl::gray,
    .specular_color = cs7gvx_utils::gl::gray};

} // namespace

namespace cs7gv3::global {

inline cs7gvx_utils::gl::camera_t camera(glm::vec3(0.0f, 1.0f, 8.0f));

inline cs7gvx_utils::gl::shader_t phong_shader(
    "shader/base.vs", "shader/phong.fs",
    std::make_shared<cs7gvx_utils::gl::phong_profile_t>(material, light));

inline cs7gvx_utils::gl::shader_t gooch_shader(
    "shader/base.vs", "shader/gooch.fs",
    std::make_shared<cs7gvx_utils::gl::gooch_profile_t>(material, light));

inline cs7gvx_utils::gl::shader_t font_shader("shader/font.vs",
                                              "shader/font.fs");

inline cs7gv3::teapot_t teapot1({-1.5, 0, 0}, &phong_shader, &camera);
inline cs7gv3::teapot_t teapot2({1.5, 0, 0}, &gooch_shader, &camera);

inline void init() {
  phong_shader.build();
  gooch_shader.build();
  font_shader.build();

  teapot1.init();
  teapot2.init();
}

} // namespace cs7gv3::global
