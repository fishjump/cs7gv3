#pragma once

#include <cs7gvx_utils.hpp>

namespace cs7gv3 {

class teapot_t : public cs7gvx_utils::gl::model_t {
public:
  teapot_t(const glm::vec3 &init_pos, cs7gvx_utils::gl::shader_t *shader,
           cs7gvx_utils::gl::camera_t *camera,
           float aspect_ratio = 800.0f / 600.0f, bool gamma_correction = false)
      : cs7gvx_utils::gl::model_t("model/teapot.obj", shader, camera,
                                  aspect_ratio, gamma_correction),
        _init_pos(init_pos) {}

  void init() override {
    model_t::init();
    _transform_mat = translate(_init_pos);
  }

  void update() override { _transform_mat = rotate(1, {0, 1, 0}); }

private:
  glm::vec3 _init_pos;
};

} // namespace cs7gv3
