#pragma once

#include "figine/figine.hpp"
#include "teapot.hpp"

namespace cs7gv3::ass1 {

extern teapot_t teapot[3];

constexpr uint8_t cook_torrance_vs[] = R"(
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

constexpr uint8_t cook_torrance_fs[] = R"(
#version 330 core

in vec3 frag_pos;
in vec3 normal;

out vec4 frag_color;

// material
uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

// lights
uniform vec3 view_pos;

uniform vec3 light_position;
uniform vec3 light_color;

const float PI = 3.14159265359;

float distribution_GGX(vec3 N, vec3 H, float roughness) {
  float a = roughness * roughness;
  float a2 = a * a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH * NdotH;

  float nom = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return nom / denom;
}

float geometry_schlick_GGX(float NdotV, float roughness) {
  float r = (roughness + 1.0);
  float k = (r * r) / 8.0;

  float nom = NdotV;
  float denom = NdotV * (1.0 - k) + k;

  return nom / denom;
}

float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness) {
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx2 = geometry_schlick_GGX(NdotV, roughness);
  float ggx1 = geometry_schlick_GGX(NdotL, roughness);

  return ggx1 * ggx2;
}

vec3 fresnel_schlick(float cosTheta, vec3 F0) {
  return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main() {
  vec3 N = normalize(normal);
  vec3 V = normalize(view_pos - frag_pos);
  vec3 L = normalize(light_position - frag_pos);
  vec3 H = normalize(V + L);

  // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
  // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
  vec3 F0 = vec3(0.04);
  F0 = mix(F0, albedo, metallic);

  // Cook-Torrance BRDF
  float NDF = distribution_GGX(N, H, roughness);
  float G = geometry_smith(N, V, L, roughness);
  vec3 F = fresnel_schlick(clamp(dot(H, V), 0.0, 1.0), F0);

  vec3 numerator = NDF * G * F;
  float denominator = max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
  vec3 specular = numerator / denominator;

  // kS is equal to Fresnel
  vec3 kS = F;
  // for energy conservation, the diffuse and specular light can't
  // be above 1.0 (unless the surface emits light); to preserve this
  // relationship the diffuse component (kD) should equal 1.0 - kS.
  vec3 kD = vec3(1.0) - kS;
  // multiply kD by the inverse metalness such that only non-metals 
  // have diffuse lighting, or a linear blend if partly metal (pure metals
  // have no diffuse light).
  kD *= 1.0 - metallic;	  

  // scale light by NdotL
  float NdotL = max(dot(N, L), 0.0);        

  // add to outgoing radiance Lo
  // note that we already multiplied the BRDF by the Fresnel (kS) 
  // so we won't multiply by kS again
  vec3 Lo = (kD * albedo / PI + specular) * light_color * NdotL;

  vec3 ambient = vec3(0.03) * albedo * ao;

  vec3 color = ambient + Lo;
  color = color / (color + vec3(1.0));
  color = pow(color, vec3(1.0 / 2.2));

  frag_color = vec4(color, 1.0);
}
)";

class cook_torrance_shader_t final : public figine::core::shader_if {
public:
  cook_torrance_shader_t()
      : figine::core::shader_if(cook_torrance_vs, cook_torrance_fs) {}
};

class cook_torrance_console_t final : public figine::imnotgui::window_t {
public:
  virtual void refresh() final {
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImGui::Begin("cook_torrance light console");
    ImGui::ColorEdit3("albedo", (float *)&teapot[2].albedo);
    ImGui::SliderFloat("metallic", &teapot[2].metallic, 0.0f, 1.0f);
    ImGui::SliderFloat("roughness", &teapot[2].roughness, 0.0f, 1.0f);
    ImGui::SliderFloat("ao", &teapot[2].ao, 0.0f, 1.0f);

    ImGui::SliderFloat3("light_position", (float *)&teapot[2].light.position,
                        -100.0f, 100.0f);
    ImGui::ColorEdit3("light_color", (float *)&teapot[2].light.diffuse_color);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();
  }
};

} // namespace cs7gv3::ass1
