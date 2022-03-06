#pragma once

#include "figine/figine.hpp"
#include "shield.hpp"

namespace cs7gv3::ass4 {

extern shield_t shield;

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
#extension GL_ARB_texture_query_lod : enable

in vec3 frag_pos;
in vec3 normal;
in vec2 texture_coordinate;

out vec4 frag_color;

uniform bool use_mip;
uniform float mipmap_level;

uniform vec3 view_pos;
uniform sampler2D texture_diffuse1;

void main() {
  if (use_mip) {
    frag_color = vec4(texture(texture_diffuse1, texture_coordinate).rgb, 1.0);
  } else {
    frag_color = textureLod(texture_diffuse1, texture_coordinate, mipmap_level);
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

    ImGui::Begin("assignmeng4 console");

    ImGui::Checkbox("use mip", &shield.use_mip);
    if (!shield.use_mip) {
      ImGui::SliderFloat("LOD", &shield.mipmap_level, 0.0f, 10.0f);
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();
  }
};

} // namespace cs7gv3::ass4
