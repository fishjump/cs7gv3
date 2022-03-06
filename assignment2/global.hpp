#pragma once

#include "figine/builtin/object/skybox.hpp"
#include "sphere.hpp"

namespace cs7gv3::ass1 {

inline figine::core::camera_t camera({0.0f, 0.0f, 0.3f});

inline sphere_console_t sphere_console;
inline figine::builtin::object::skybox_t
    skybox({"model/skybox/right.jpg", "model/skybox/left.jpg",
            "model/skybox/top.jpg", "model/skybox/bottom.jpg",
            "model/skybox/front.jpg", "model/skybox/back.jpg"},
           &camera);
inline sphere_t sphere({0, 0, 0}, &camera);

inline void init() {
  sphere.init();
  skybox.init();
}

} // namespace cs7gv3::ass1
