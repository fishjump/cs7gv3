#pragma once

#include "cook_torrance_shader.hpp"
#include "gooch_shader.hpp"
#include "phong_shader.hpp"
#include "teapot.hpp"

namespace cs7gv3::ass1 {

inline figine::core::camera_t camera({0, 1, 9});

inline phong_shader_t phong_shader;
inline phong_console_t phong_console;

inline gooch_shader_t gooch_shader;
inline gooch_console_t gooch_console;

inline cook_torrance_shader_t cook_torrance_shader;
inline cook_torrance_console_t cook_torrance_console;

inline teapot_t teapot[] = {
    teapot_t({0, 0, 0}, &camera),
    teapot_t({0, 0, 0}, &camera),
    teapot_t({3, 0, 0}, &camera),
};

inline void init() {
  phong_shader.build();
  gooch_shader.build();
  cook_torrance_shader.build();
  for (int i = 0; i < 3; i++) {
    teapot[i].init();
  }
}

} // namespace cs7gv3::ass1
