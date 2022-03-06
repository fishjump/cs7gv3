#pragma once

#include "figine/builtin/object/skybox.hpp"
#include "figine/figine.hpp"

namespace cs7gv3::ass1 {

extern figine::builtin::object::skybox_t skybox;
extern figine::core::camera_t camera;

constexpr uint8_t sphere_vs[] = R"(
#version 330 core

layout(location = 0) in vec3 pos_in;
layout(location = 1) in vec3 normal_in;

out vec3 frag_pos;
out vec3 normal;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main() {
    frag_pos = vec3(transform * vec4(pos_in, 1.0));
    normal = mat3(transpose(inverse(transform))) * normal_in;
    gl_Position = projection * view * vec4(frag_pos, 1.0);
}
)";

constexpr uint8_t sphere_fs[] = R"(
#version 330 core
out vec4 frag_color;

in vec3 normal;
in vec3 frag_pos;

uniform vec3 camera_pos;
uniform samplerCube skybox;
uniform bool use_reflect;
uniform bool use_refract;
uniform float refract_ratio;

vec3 cal_reflect() {
  vec3 I = normalize(frag_pos - camera_pos);
  vec3 R = reflect(I, normalize(normal));

  return R;
}

vec3 cal_refract() {
  vec3 I = normalize(frag_pos - camera_pos);
  vec3 R = refract(I, normalize(normal), refract_ratio);

  return R;
}

void main() {
    int cnt = 0;
    vec4 reflect_res = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 refract_res = vec4(0.0, 0.0, 0.0, 0.0);
    if (use_reflect) {
      reflect_res = vec4(texture(skybox, cal_reflect()).rgb, 1.0);
      cnt++;
    }

    if (use_refract) {
      refract_res = vec4(texture(skybox, cal_refract()).rgb, 1.0);
      cnt++;
    }

    frag_color = (reflect_res + refract_res) / cnt;
}
)";

class sphere_t : public figine::core::object_t {
public:
  sphere_t(const glm::vec3 &init_pos, figine::core::camera_t *camera,
           bool gamma_correction = false)
      : figine::core::object_t("model/sphere.off", camera, gamma_correction),
        _init_pos(init_pos) {}

  float refract_ratio = 1.0f;
  bool use_reflect = true;
  bool use_refract = true;

  void init() override {
    object_t::init();
    _shader.build();
    transform = translate(_init_pos);
    transform = scale(glm::vec3(0.1f));

    for (const auto &mesh : object_t::_meshes) {
      glBindVertexArray(mesh.vao);
      defer(glBindVertexArray(0));
      glGenTextures(1, &_box_texture);
      glBindTexture(GL_TEXTURE_CUBE_MAP, _box_texture);

      for (size_t i = 0; i < skybox.faces.size(); i++) {
        int width = 0, height = 0, nr_components = 0;
        uint8_t *data = stbi_load(skybox.faces[i].c_str(), &width, &height,
                                  &nr_components, 0);
        defer(stbi_image_free(data));
        if (data) {
          glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width,
                       height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else {
          LOG_ERR("cubemap texture failed to load at path: %s",
                  skybox.faces[i].c_str());
        }
      }

      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
  }

  void update() override { object_t::update(); }

  void apply_uniform(const figine::core::shader_if &shader) override {
    object_t::apply_uniform(shader);
    shader.set_uniform("use_reflect", use_reflect);
    shader.set_uniform("use_refract", use_refract);
    shader.set_uniform("refract_ratio", refract_ratio);
    shader.set_uniform("camera_pos", camera->position);
  }

  void loop() {
    update();
    apply_uniform(_shader);

    for (const auto &mesh : object_t::_meshes) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_CUBE_MAP, _box_texture);
      mesh.draw(_shader);
    }
  }

private:
  class sphere_shader_t final : public figine::core::shader_if {
  public:
    sphere_shader_t() : figine::core::shader_if(sphere_vs, sphere_fs) {}
  };

  glm::vec3 _init_pos;
  sphere_shader_t _shader;
  GLuint _box_texture;
};

extern sphere_t sphere;

class sphere_console_t final : public figine::imnotgui::window_t {
public:
  // air
  float media1 = 1.0f;
  // water
  float media2 = 1.33f;

  virtual void refresh() final {
    ImGui::Begin("sphere console");

    ImGui::Checkbox("use reflect", &sphere.use_reflect);
    ImGui::Checkbox("use refract", &sphere.use_refract);

    if (sphere.use_refract) {
      ImGui::InputFloat("media1", &media1, 0.0f, 100.0f);
      ImGui::InputFloat("media2", &media2, 0.0f, 100.0f);
      sphere.refract_ratio = media1 / media2;
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();
  }
};

} // namespace cs7gv3::ass1
