#pragma once

#include "figine/figine.hpp"

namespace cs7gv3::ass1 {

class teapot_t : public figine::core::object_t {
public:
  teapot_t(const glm::vec3 &init_pos, figine::core::camera_t *camera,
           bool gamma_correction = false)
      : figine::core::object_t("model/teapot.obj", camera, gamma_correction),
        _init_pos(init_pos) {}

  // phong
  figine::builtin::shader::material_t material = {
      .shininess = 16.0f,
      .ambient_color = glm::vec3(0.1f),
      .diffuse_color = glm::vec3(0.5f),
      .specular_color = glm::vec3(1.0f),
  };

  figine::builtin::shader::light_t light = {
      .position = {0.0f, 2.0f, 8.0f},
      .ambient_color = glm::vec3(0.1f),
      .diffuse_color = glm::vec3(1.0f),
      .specular_color = glm::vec3(1.0f),
  };

  // gooch
  GLfloat a = 0.2;
  GLfloat b = 0.6;
  glm::vec3 k_blue = {0.0f, 0.0f, 0.4f};
  glm::vec3 k_yellow = {0.4f, 0.4f, 0.0f};

  // cook-torrance
  glm::vec3 albedo = glm::vec3(1.0f);
  GLfloat metallic;
  GLfloat roughness;
  GLfloat ao;

  void init() override {
    object_t::init();
    transform = translate(_init_pos);
  }

  void update() override {
    object_t::update();
    transform = rotate_around(glm::radians(1.0f), {0, 1, 0});
  }

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

    shader.set_uniform("a", a);
    shader.set_uniform("b", b);
    shader.set_uniform("k_blue", k_blue);
    shader.set_uniform("k_yellow", k_yellow);

    shader.set_uniform("albedo", albedo);
    shader.set_uniform("metallic", metallic);
    shader.set_uniform("roughness", roughness);
    shader.set_uniform("ao", ao);

    shader.set_uniform("light_position", light.position);
    shader.set_uniform("light_color", light.diffuse_color);
  }

private:
  glm::vec3 _init_pos;
};

} // namespace cs7gv3::ass1
