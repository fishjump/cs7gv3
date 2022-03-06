#pragma once

#include "phong_shader.hpp"
#include "shield.hpp"

namespace cs7gv3::ass3 {

inline figine::core::camera_t camera({0, 1, 9});

inline phong_shader_t phong_shader;
inline phong_console_t phong_console;

inline shield_t shield = shield_t({0, 0, 0}, &camera);

inline void init() {
  phong_shader.build();
  shield.init();
}

} // namespace cs7gv3::ass3
