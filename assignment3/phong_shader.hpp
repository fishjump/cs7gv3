#pragma once

#include "figine/figine.hpp"
#include "shield.hpp"

namespace cs7gv3::ass3 {

extern shield_t shield;

constexpr uint8_t phong_vs[] = R"(
#version 330 core

layout(location = 0) in vec3 pos_in;
layout(location = 1) in vec3 normal_in;
layout(location = 2) in vec2 texture_coordinate_in;
layout(location = 3) in vec3 tangent_in;
layout(location = 4) in vec3 bitangent_in;

struct material_t {
  float shininess;
  vec3 ambient_color;
  vec3 diffuse_color;
  vec3 specular_color;
};

struct light_t {
  vec3 position;
  vec3 ambient_color;
  vec3 diffuse_color;
  vec3 specular_color;
};

struct vs_out_t {
  vec3 frag_pos;
  vec3 normal;
  vec2 texture_coordinate;
  vec3 tangent_light_pos;
  vec3 tangent_view_pos;
  vec3 tangent_frag_pos; 
};

out vs_out_t vs_out;

uniform material_t material;
uniform light_t light;

uniform mat4 view;
uniform vec3 view_pos;
uniform mat4 transform;
uniform mat4 projection;

void main() {
  vs_out.frag_pos = vec3(transform * vec4(pos_in, 1.0));
  vs_out.texture_coordinate = texture_coordinate_in;

  mat3 normal = transpose(inverse(mat3(transform)));
  vec3 T = normalize(vec3(transform * vec4(tangent_in,   0.0)));
  vec3 B = normalize(vec3(transform * vec4(bitangent_in, 0.0)));
  vec3 N = normalize(vec3(transform * vec4(normal_in,    0.0)));
  mat3 TBN = transpose(mat3(T, B, N));

  vs_out.normal = normal * normal_in;
  vs_out.tangent_light_pos = TBN * light.position;
  vs_out.tangent_view_pos  = TBN * view_pos;
  vs_out.tangent_frag_pos  = TBN * vs_out.frag_pos;

  gl_Position = projection * view * transform * vec4(pos_in, 1.0);
}
)";

constexpr uint8_t phong_fs[] = R"(
#version 330 core

struct material_t {
  float shininess;
  vec3 ambient_color;
  vec3 diffuse_color;
  vec3 specular_color;
};

struct light_t {
  vec3 position;
  vec3 ambient_color;
  vec3 diffuse_color;
  vec3 specular_color;
};

struct vs_out_t {
  vec3 frag_pos;
  vec3 normal;
  vec2 texture_coordinate;
  vec3 tangent_light_pos;
  vec3 tangent_view_pos;
  vec3 tangent_frag_pos;
};

in vs_out_t vs_out;

out vec4 frag_color;

uniform bool use_norm;

uniform vec3 view_pos;
uniform material_t material;
uniform light_t light;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;

vec4 cal_with_norm() {
  vec3 norm = texture(texture_normal1, vs_out.texture_coordinate).rgb;
  norm = normalize(norm * 2.0 - 1.0);

  vec3 view_direction = normalize(vs_out.tangent_view_pos - vs_out.tangent_frag_pos);
  vec3 light_direction = normalize(vs_out.tangent_light_pos - vs_out.tangent_frag_pos);
  vec3 reflect_direction = reflect(-light_direction, norm);
  vec3 halfway_direction = normalize(light_direction + view_direction);

  vec3 color = texture(texture_diffuse1, vs_out.texture_coordinate).rgb;

  // ambient
  vec3 ambient = color * light.ambient_color * material.ambient_color;

  // diffuse 
  float diff = max(dot(norm, light_direction), 0.0);
  vec3 diffuse = diff * color * light.diffuse_color * material.diffuse_color;

  // specular
  float spec = pow(max(dot(norm, halfway_direction), 0.0), material.shininess);
  vec3 specular = spec * light.specular_color * material.specular_color;

  return vec4(ambient + diffuse + specular, 1.0);
}

vec4 cal_regular() {
  vec3 norm = normalize(vs_out.normal);

  vec3 view_direction = normalize(view_pos - vs_out.frag_pos);
  vec3 light_direction = normalize(vs_out.frag_pos - light.position);
  vec3 reflect_direction = reflect(light_direction, norm);

  vec3 color = texture(texture_diffuse1, vs_out.texture_coordinate).rgb;

  // ambient
  vec3 ambient = color * light.ambient_color * material.ambient_color;

  // diffuse 
  float diff = max(dot(norm, -light_direction), 0.0);
  vec3 diffuse = diff * color * light.diffuse_color * material.diffuse_color;

  // specular
  float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
  vec3 specular = spec * light.specular_color * material.specular_color;

  return vec4(ambient + diffuse + specular, 1.0);
}

void main() {
  if (use_norm) {
    frag_color = cal_with_norm();
  } else {
    frag_color = cal_regular();
  }
}
)";

class phong_shader_t final : public figine::core::shader_if {
public:
  phong_shader_t() : figine::core::shader_if(phong_vs, phong_fs) {}
};

class phong_console_t final : public figine::imnotgui::window_t {
public:
  virtual void refresh() final {
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImGui::Begin("assignmeng3 console");

    ImGui::Checkbox("use norm", &shield.use_norm);

    ImGui::Text("material: ");
    ImGui::SliderFloat("m.shininess", &shield.material.shininess, 0.0f, 100.0f);
    ImGui::ColorEdit3("m.ambient_color",
                      (float *)&shield.material.ambient_color);
    ImGui::ColorEdit3("m.diffuse_color",
                      (float *)&shield.material.diffuse_color);
    ImGui::ColorEdit3("m.specular_color",
                      (float *)&shield.material.specular_color);

    ImGui::Text("light: ");
    ImGui::SliderFloat3("l.position", (float *)&shield.light.position, -100.0f,
                        100.0f);
    ImGui::ColorEdit3("l.ambient_color", (float *)&shield.light.ambient_color);
    ImGui::ColorEdit3("l.diffuse_color", (float *)&shield.light.diffuse_color);
    ImGui::ColorEdit3("l.specular_color",
                      (float *)&shield.light.specular_color);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();
  }
};

} // namespace cs7gv3::ass3
