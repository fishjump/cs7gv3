#pragma once

#include "figine/figine.hpp"
#include "teapot.hpp"

namespace cs7gv3::ass1 {

extern teapot_t teapot[3];

constexpr uint8_t gooch_vs[] = R"(
#version 330 core

layout(location = 0) in vec3 pos_in;
layout(location = 1) in vec3 normal_in;
layout(location = 2) in vec2 texture_coordinate_in;

out vec3 frag_pos;
out vec3 normal;
out vec2 texture_coordinate;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main() {
    texture_coordinate = texture_coordinate_in;
    frag_pos = vec3(transform * vec4(pos_in, 1.0));
    normal = mat3(transpose(inverse(transform))) * normal_in;

    gl_Position = projection * view * vec4(frag_pos, 1.0);
}
)";

constexpr uint8_t gooch_fs[] = R"(
#version 330 core

struct material_t {
  vec3 ambient_color;
  vec3 diffuse_color;
  vec3 specular_color;
  float shininess;
};

struct light_t {
  vec3 position;
  vec3 ambient_color;
  vec3 diffuse_color;
  vec3 specular_color;
};

in vec3 frag_pos;
in vec3 normal;

out vec4 frag_color;

uniform vec3 view_pos;
uniform material_t material;
uniform light_t light;

uniform float a;
uniform float b;
uniform vec3 k_blue;
uniform vec3 k_yellow;

vec3 k_cool_f(vec3 k_d) {
  return k_blue + a * k_d;
}

vec3 k_warm_f(vec3 k_d) {
  return k_yellow + b * k_d;
}

void main() {
  vec3 norm = normalize(normal);
  vec3 view_direction = normalize(view_pos - frag_pos);
  vec3 light_direction = normalize(light.position - frag_pos);
  vec3 reflect_direction = reflect(-light_direction, norm);

  // ambient
  vec3 ambient = light.ambient_color * material.ambient_color;

  // diffuse 
  float diff = dot(-light_direction, norm);
  float dot_1_2 = (1.0 + diff) / 2.0;
  vec3 diffuse_color = diff * light.diffuse_color * material.diffuse_color;
  vec3 k_cool = k_cool_f(diffuse_color);
  vec3 k_warm = k_warm_f(diffuse_color);
  vec3 diffuse = dot_1_2 * k_cool + (1.0 - dot_1_2) * k_warm;

  // specular
  float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
  vec3 specular = spec * light.specular_color * material.specular_color;

  frag_color = vec4(ambient + diffuse + specular, 1.0);
}
)";

class gooch_shader_t final : public figine::core::shader_if {
public:
  gooch_shader_t() : figine::core::shader_if(gooch_vs, gooch_fs) {}
};

class gooch_console_t final : public figine::imnotgui::window_t {
public:
  virtual void refresh() final {
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImGui::Begin("gooch light console");
    ImGui::Text("material: ");
    ImGui::SliderFloat("m.shininess", &teapot[1].material.shininess, 0.0f,
                       100.0f);
    ImGui::ColorEdit3("m.ambient_color",
                      (float *)&teapot[1].material.ambient_color);
    ImGui::ColorEdit3("m.diffuse_color",
                      (float *)&teapot[1].material.diffuse_color);
    ImGui::ColorEdit3("m.specular_color",
                      (float *)&teapot[1].material.specular_color);

    ImGui::Text("light: ");
    ImGui::SliderFloat3("l.position", (float *)&teapot[1].light.position,
                        -100.0f, 100.0f);
    ImGui::ColorEdit3("l.ambient_color",
                      (float *)&teapot[1].light.ambient_color);
    ImGui::ColorEdit3("l.diffuse_color",
                      (float *)&teapot[1].light.diffuse_color);
    ImGui::ColorEdit3("l.specular_color",
                      (float *)&teapot[1].light.specular_color);

    ImGui::Text("gooch: ");
    ImGui::SliderFloat("alpha", &teapot[1].a, 0, 1);
    ImGui::SliderFloat("beta", &teapot[1].b, 0, 1);
    ImGui::ColorEdit3("k_blue", (float *)&teapot[1].k_blue);
    ImGui::ColorEdit3("k_yellow", (float *)&teapot[1].k_yellow);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();
  }
};

} // namespace cs7gv3::ass1
