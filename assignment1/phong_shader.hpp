#pragma once

#include "figine/figine.hpp"
#include "teapot.hpp"

namespace cs7gv3::ass1 {

extern teapot_t teapot[3];

constexpr uint8_t phong_vs[] = R"(
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

in vec3 frag_pos;
in vec3 normal;

out vec4 frag_color;

uniform vec3 view_pos;
uniform material_t material;
uniform light_t light;

void main() {
    vec3 norm = normalize(normal);
    vec3 view_direction = normalize(view_pos - frag_pos);
    vec3 light_direction = normalize(frag_pos - light.position);
    vec3 reflect_direction = reflect(light_direction, norm);

    // ambient
    vec3 ambient = light.ambient_color * material.ambient_color;

    // diffuse 
    float diff = max(dot(norm, -light_direction), 0.0);
    vec3 diffuse = diff * light.diffuse_color * material.diffuse_color;

    // specular
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
    vec3 specular = spec * light.specular_color * material.specular_color;

    frag_color = vec4(ambient + diffuse + specular, 1.0);
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

    ImGui::Begin("phong light console");
    ImGui::Text("material: ");
    ImGui::SliderFloat("m.shininess", &teapot[0].material.shininess, 0.0f,
                       100.0f);
    ImGui::ColorEdit3("m.ambient_color",
                      (float *)&teapot[0].material.ambient_color);
    ImGui::ColorEdit3("m.diffuse_color",
                      (float *)&teapot[0].material.diffuse_color);
    ImGui::ColorEdit3("m.specular_color",
                      (float *)&teapot[0].material.specular_color);

    ImGui::Text("light: ");
    ImGui::SliderFloat3("l.position", (float *)&teapot[0].light.position,
                        -100.0f, 100.0f);
    ImGui::ColorEdit3("l.ambient_color",
                      (float *)&teapot[0].light.ambient_color);
    ImGui::ColorEdit3("l.diffuse_color",
                      (float *)&teapot[0].light.diffuse_color);
    ImGui::ColorEdit3("l.specular_color",
                      (float *)&teapot[0].light.specular_color);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();
  }
};

} // namespace cs7gv3::ass1
