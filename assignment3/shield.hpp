#pragma once

#include "figine/figine.hpp"

namespace cs7gv3::ass3 {

class shield_t : public figine::core::object_t {
public:
  shield_t(const glm::vec3 &init_pos, figine::core::camera_t *camera,
           bool gamma_correction = false)
      : figine::core::object_t("model/shield.obj", camera, gamma_correction),
        _init_pos(init_pos) {}

  bool use_norm = true;

  // phong
  figine::builtin::shader::material_t material = {
      .shininess = 16.0f,
      .ambient_color = glm::vec3(1.0f),
      .diffuse_color = glm::vec3(1.0f),
      .specular_color = glm::vec3(1.0f),
  };

  figine::builtin::shader::light_t light = {
      .position = {0.0f, 0.0f, 0.0f},
      .ambient_color = glm::vec3(1.0f),
      .diffuse_color = glm::vec3(1.0f),
      .specular_color = glm::vec3(1.0f),
  };

  void init() override {
    object_t::init();
    transform = translate(_init_pos);
    transform = scale(glm::vec3(2.0f));
    transform =
        rotate_around(glm::radians(180.0f), glm::vec3{0.0f, 1.0f, 0.0f});
  }

  void update() override { object_t::update(); }

  void apply_uniform(const figine::core::shader_if &shader) override {
    object_t::apply_uniform(shader);

    shader.set_uniform("light.position", light.position);
    shader.set_uniform("light.ambient_color", light.ambient_color);
    shader.set_uniform("light.diffuse_color", light.diffuse_color);
    shader.set_uniform("light.specular_color", light.specular_color);

    shader.set_uniform("material.shininess", material.shininess);
    shader.set_uniform("material.ambient_color", material.ambient_color);
    shader.set_uniform("material.diffuse_color", material.diffuse_color);
    shader.set_uniform("material.specular_color", material.specular_color);

    shader.set_uniform("light_position", light.position);
    shader.set_uniform("light_color", light.diffuse_color);

    shader.set_uniform("use_norm", use_norm);
  }

private:
  glm::vec3 _init_pos;
};

} // namespace cs7gv3::ass3
