#pragma once

#include "figine/figine.hpp"

namespace cs7gv3::ass4 {

class shield_t : public figine::core::object_t {
public:
  shield_t(const glm::vec3 &init_pos, figine::core::camera_t *camera,
           bool gamma_correction = false)
      : figine::core::object_t("model/shield.obj", camera, gamma_correction),
        _init_pos(init_pos) {}

  bool use_mip = true;
  float mipmap_level = 3;

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

    shader.set_uniform("use_mip", use_mip);
    shader.set_uniform("mipmap_level", mipmap_level);
  }

private:
  glm::vec3 _init_pos;
};

} // namespace cs7gv3::ass4
