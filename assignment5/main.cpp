#include "figine/figine.hpp"

#include "teapot.hpp"

#include <limits>
#include <sstream>

#include <glm/ext/matrix_projection.hpp>
#include <glm/gtx/string_cast.hpp>

constexpr uint8_t paint_vs[] = R"(
#version 330 core

layout(location = 0) in vec3 position_in;
layout(location = 1) in vec3 normal_in;

out vec3 frag_pos;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    frag_pos = vec3(model * vec4(position_in, 1.0));
    normal = mat3(transpose(inverse(model))) * normal_in;

    gl_Position = projection * view * vec4(frag_pos, 1.0);
}
)";

constexpr uint8_t paint_fs[] = R"(
#version 330 core

out vec4 frag_color;

void main() {
    frag_color = vec4(1.0, 1.0, 1.0, 1.0);
}
)";

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

#define MAX_LIGHTS 128

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

uniform int n;
uniform vec3 view_pos;
uniform material_t material;
uniform light_t light[MAX_LIGHTS];

void main() {
    vec3 ambient = vec3(0.3f, 0.3f, 0.3f);
    vec3 diffuse = vec3(0.0f, 0.0f, 0.0f);
    vec3 specular = vec3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < n; i++) {
      vec3 norm = normalize(normal);
      vec3 view_direction = normalize(view_pos - frag_pos);
      vec3 light_direction = normalize(frag_pos - light[i].position);
      float light_length = length(frag_pos - light[i].position);
      vec3 reflect_direction = reflect(light_direction, norm);

      float diff = max(dot(norm, -light_direction), 0.0);
      diffuse += (1 - light_length / 100) * diff * light[i].diffuse_color * material.diffuse_color;

      float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
      specular += (1 - light_length / 100) * spec * light[i].specular_color * material.specular_color;
    }

    frag_color = vec4(ambient + diffuse + specular, 1.0);
}
)";

// ============= Global Variables =============

namespace cs7gv3::ass5 {

float current = 0.0f;
float last = 0.0f;
float delta = 0.0f;

const uint32_t &win_height = figine::global::win_mgr::height;
const uint32_t &win_width = figine::global::win_mgr::width;

figine::core::camera_t camera({0.0f, 0.1f, 0.3f});

figine::core::shader_if paint_shader(paint_vs, paint_fs);
figine::core::shader_if teapot_shader(phong_vs, phong_fs);

teapot_t teapot({0, 0, 0}, &camera);

const glm::vec3 circle_scale{0.005f, 0.005f, 0.005f};

std::vector<glm::vec3> selected_vertexes;
std::vector<glm::vec3> circle_centers;

std::vector<glm::vec3> light_pos;

class phong_console_t final : public figine::imnotgui::window_t {
public:
  bool preview_enable = false;
  float light_length = 1.0f;

  virtual void refresh() final {
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImGui::Begin("console");

    size_t del_idx = -1;
    for (size_t i = 0; i < light_pos.size(); i++) {
      std::stringstream ss;
      ss << "l.position[" << i << "]";
      ImGui::SliderFloat3(ss.str().c_str(), (float *)&light_pos[i], -100.0f,
                          100.f);

      std::stringstream ss1;
      ss1 << "delete " << i;
      if (ImGui::Button(ss1.str().c_str())) {
        del_idx = i;
      }
    }

    if (del_idx != -1) {
      light_pos.erase(light_pos.begin() + del_idx);
    }

    if (ImGui::Button("reset")) {
      light_pos.clear();
    }

    ImGui::Checkbox("enable preview", &preview_enable);

    ImGui::End();
  }
};

phong_console_t console;

} // namespace cs7gv3::ass5

// =========== End Global Variables ===========

void init() {
  using namespace cs7gv3::ass5;

  paint_shader.build();
  teapot_shader.build();

  teapot.init();
  teapot.scale(glm::vec3(0.01f));
}

void mouse_event_cbk(GLFWwindow *window, double x_pos_in, double y_pos_in) {
  using namespace cs7gv3::ass5;

  static int last_status = GLFW_RELEASE;
  // int current_status = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
  int current_status = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

  double _x = 0, _y = 0;
  glfwGetCursorPos(window, &_x, &_y);

  float x = _x, y = win_height - _y, z = 0;
  glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);

  if (1.0 - z < 0.001 || z < 0.001) {
    z = 0.5f;
  }

  auto proj =
      glm::perspective(glm::radians(camera.zoom),
                       figine::global::win_mgr::aspect_ratio(), 0.1f, 100.0f);
  glm::mat4 teapot_model = teapot.transform;
  glm::mat4 circle_model = glm::scale(glm::mat4(1.0f), circle_scale);
  glm::vec3 gl_world_pos =
      glm::unProject(glm::vec3{x, y, z}, camera.view_matrix() * teapot_model,
                     proj, glm::vec4{0.0f, 0.0f, win_width, win_height});

  glm::vec3 gl_show_pos =
      glm::unProject(glm::vec3{x, y, 0}, camera.view_matrix() * circle_model,
                     proj, glm::vec4{0.0f, 0.0f, win_width, win_height});

  size_t idx = 0;
  float min = std::numeric_limits<float>::max();
  for (size_t i = 0; i < teapot._meshes[0]._vertices.size(); i += 3) {
    glm::vec3 P_i = teapot._meshes[0]._vertices[i].position;
    glm::vec3 X = gl_world_pos;
    float tmp = glm::abs(glm::distance(P_i, X));
    if (tmp < min) {
      min = tmp;
      idx = i;
    }
  }

  glm::vec3 frag_pos =
      glm::vec3(teapot.transform *
                glm::vec4(teapot._meshes[0]._vertices[idx].position, 1.0));
  glm::vec3 R = camera.position - frag_pos; // R approximately equals to V
  glm::vec3 N = glm::mat3(glm::transpose(glm::inverse(teapot.transform))) *
                teapot._meshes[0]._vertices[idx].normal;
  glm::vec3 I = -glm::reflect(R, N);
  glm::vec3 L = I + frag_pos;
  if (console.preview_enable) {
    if (light_pos.empty()) {
      light_pos.push_back(L);
    } else {
      light_pos[0] = (L);
    }
  }

  glm::vec3 center(0.0f);
  if (!selected_vertexes.empty()) {
    for (const auto &item : selected_vertexes) {
      center += item;
    }
    center /= selected_vertexes.size();
  }
  LOG_INFO("%s", glm::to_string(center).c_str());

  if (current_status == GLFW_PRESS && last_status == GLFW_RELEASE) {
    // key down
    circle_centers.push_back(gl_show_pos);
    selected_vertexes.push_back(gl_world_pos);
  } else if (current_status == GLFW_PRESS && last_status == GLFW_PRESS) {
    // holding
    circle_centers.push_back(gl_show_pos);

    selected_vertexes.push_back(gl_world_pos);
  } else if (current_status == GLFW_RELEASE && last_status == GLFW_PRESS) {
    if (!selected_vertexes.empty()) {
      glm::vec3 center(0.0f);
      for (const auto &item : selected_vertexes) {
        center += item;
      }
      center /= selected_vertexes.size();
      size_t idx = 0;
      float min = std::numeric_limits<float>::max();
      for (size_t i = 0; i < teapot._meshes[0]._vertices.size(); i += 3) {
        glm::vec3 P_i = teapot._meshes[0]._vertices[i].position;
        glm::vec3 X = center;
        float tmp = glm::abs(glm::distance(P_i, X));
        if (tmp < min) {
          min = tmp;
          idx = i;
        }
      }

      glm::vec3 frag_pos =
          glm::vec3(teapot.transform *
                    glm::vec4(teapot._meshes[0]._vertices[idx].position, 1.0));
      glm::vec3 R = camera.position - frag_pos; // R approximately equals to V
      glm::vec3 N = glm::mat3(glm::transpose(glm::inverse(teapot.transform))) *
                    teapot._meshes[0]._vertices[idx].normal;
      glm::vec3 I = -glm::reflect(R, N);
      glm::vec3 L = I + frag_pos;

      light_pos.push_back(L);

      selected_vertexes.clear();
      circle_centers.clear();
    }

  } else if (current_status == GLFW_RELEASE && last_status == GLFW_RELEASE) {
    // idle
  }

  last_status = current_status;
}

void process_input(GLFWwindow *window, float delta_time) {
  using namespace cs7gv3::ass5;

  delta_time *= 0.1;

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
    camera.position =
        glm::vec4(camera.position, 1.0) *
        glm::rotate(glm::mat4(1.0f), glm::radians(-1.0f), {0.0f, 1.0f, 0.0f});
  }

  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    camera.position =
        glm::vec4(camera.position, 1.0) *
        glm::rotate(glm::mat4(1.0f), glm::radians(1.0f), {0.0f, 1.0f, 0.0f});
  }

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera.position =
        glm::vec4(camera.position, 1.0) *
        glm::rotate(glm::mat4(1.0f), glm::radians(-1.0f), {1.0f, 0.0f, 0.0f});
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera.position =
        glm::vec4(camera.position, 1.0) *
        glm::rotate(glm::mat4(1.0f), glm::radians(1.0f), {1.0f, 0.0f, 0.0f});
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera.position =
        glm::vec4(camera.position, 1.0) *
        glm::rotate(glm::mat4(1.0f), glm::radians(-1.0f), {0.0f, 1.0f, 0.0f});
  }

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera.position =
        glm::vec4(camera.position, 1.0) *
        glm::rotate(glm::mat4(1.0f), glm::radians(1.0f), {0.0f, 1.0f, 0.0f});
  }

  // if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
  //   camera.position += glm::vec3{0, -1, 0} * delta_time;
  // }

  // if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
  //   camera.position += glm::vec3{0, 1, 0} * delta_time;
  // }

  if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
    // camera.pitch += delta_time;
  }

  if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
    // camera.pitch -= delta_time;
  }

  if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
    // camera.yaw += delta_time;
    // camera.position += glm::vec3{0, -1, 0} * delta_time;
  }

  if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
    // camera.yaw -= delta_time;
    // camera.position += glm::vec3{0, 1, 0} * delta_time;
  }
}

void render_circle(const glm::vec3 &pos) {
  using namespace cs7gv3::ass5;

  static uint32_t vao = 0, vbo = 0;
  static float data[360 * 3 * 3] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f,
                                    0.0f,  0.0f,  0.5f, 0.0f};
  static bool initialized = false;
  if (!initialized) {
    for (int i = 0, j = 0; i < 360; i++) {
      // vertex 0
      data[j++] = glm::cos(glm::radians((float)i));
      data[j++] = glm::sin(glm::radians((float)i));
      data[j++] = 0.0f;

      // vertex 1
      data[j++] = glm::cos(glm::radians((float)i + 1));
      data[j++] = glm::sin(glm::radians((float)i + 1));
      data[j++] = 0.0f;

      // vertex 2
      data[j++] = 0.0;
      data[j++] = 0.0;
      data[j++] = 0.0f;
    }
    initialized = true;
  }

  if (vao == 0) {
    glGenVertexArrays(1, &vao);
  }

  if (vbo == 0) {
    glGenBuffers(1, &vbo);
  }

  glBindVertexArray(vao);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);

  paint_shader.use();
  paint_shader.set_uniform("view_pos", camera.position);
  paint_shader.set_uniform(
      "projection",
      glm::perspective(glm::radians(camera.zoom),
                       figine::global::win_mgr::aspect_ratio(), 0.1f, 100.0f));
  glm::mat4 model(1.0f);
  model = glm::scale(model, circle_scale);
  model = glm::translate(model, pos);
  paint_shader.set_uniform("model", model);
  paint_shader.set_uniform("view", camera.view_matrix());

  glDrawArrays(GL_TRIANGLES, 0, 360 * 3);
}

void render_circles() {
  using namespace cs7gv3::ass5;
  for (const auto &pos : circle_centers) {
    render_circle(pos);
  }
}

int main(int argc, char **argv) {
  using namespace cs7gv3::ass5;

  figine::global::init();

  GLFWwindow *window =
      figine::global::win_mgr::create_window(800, 600, "cs7gv5", NULL, NULL);
  glfwSetCursorPosCallback(window, mouse_event_cbk);
  init();

  figine::imnotgui::init(window);
  figine::imnotgui::register_window(&console);

  camera.lock({0, 0.1, 0});
  while (!glfwWindowShouldClose(window)) {
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    current = glfwGetTime();
    delta = current - last;
    last = current;

    process_input(window, delta);

    render_circles();

    teapot_shader.use();
    teapot_shader.set_uniform("n", light_pos.size());
    teapot_shader.set_uniform("light_length", console.light_length);
    for (size_t i = 0; i < light_pos.size(); i++) {
      std::stringstream ss;
      ss << "light[" << i << "].";
      std::string prefix = ss.str();
      teapot_shader.set_uniform(prefix + "position", light_pos[i]);
      teapot_shader.set_uniform(prefix + "ambient_color",
                                teapot.light.ambient_color);
      teapot_shader.set_uniform(prefix + "diffuse_color",
                                teapot.light.diffuse_color);
      teapot_shader.set_uniform(prefix + "specular_color",
                                teapot.light.specular_color);
    }
    teapot.loop(teapot_shader);

    figine::imnotgui::render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}
