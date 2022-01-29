#include <array>
#include <iostream>
#include <optional>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <common.hpp>
#include <gl.hpp>
#include <program_options.hpp>

namespace {

std::vector<gl::shader_t> shaders;

gl::camera_t camera(glm::vec3(0.0f, 0.0f, 3.0f));
constexpr uint32_t SCR_WIDTH = 800;
constexpr uint32_t SCR_HEIGHT = 600;

float last_x = SCR_WIDTH / 2.0f;
float last_y = SCR_HEIGHT / 2.0f;
bool first_mouse = true;
float delta_time = 0.0f;
float last_frame = 0.0f;

void process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera.process_keyboard(gl::camera_movement_t::FORWARD, delta_time);
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera.process_keyboard(gl::camera_movement_t::BACKWARD, delta_time);
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera.process_keyboard(gl::camera_movement_t::LEFT, delta_time);
  }

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera.process_keyboard(gl::camera_movement_t::RIGHT, delta_time);
  }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double x_pos_in, double y_pos_in) {
  float x_pos = static_cast<float>(x_pos_in);
  float y_pos = static_cast<float>(y_pos_in);

  if (first_mouse) {
    last_x = x_pos;
    last_y = y_pos;
    first_mouse = false;
  }

  float x_offset = x_pos - last_x;
  float y_offset = last_y - y_pos;

  last_x = x_pos;
  last_y = y_pos;

  camera.process_mouse_movement(x_offset, y_offset);
}

void scroll_callback(GLFWwindow *window, double x_offset, double y_offset) {
  camera.process_mouse_scroll(static_cast<float>(y_offset));
}

} // namespace

int main(int argc, char **argv) {
  auto opts = cs7gv3::parse_opts(argc, argv);

  gl::init();

  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "cs7gv3", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // create shaders
  shaders.push_back(gl::shader_t(opts.vert.value(), opts.frag.value(), true));

  // build shaders
  for (auto &shader : shaders) {
    auto res = shader.build();
    if (res.err != std::nullopt) {
      LOG_ERR(res.err.value());
      return 1;
    }
  }

  gl::model_t model(opts.model.value());

  while (!glfwWindowShouldClose(window)) {
    float current_frame = static_cast<float>(glfwGetTime());
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    process_input(window);

    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaders[0].use();

    glm::mat4 projection_u =
        glm::perspective(glm::radians(camera.zoom()),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    shaders[0].set_uniform("projection_u", projection_u);

    glm::mat4 view_u = camera.view_matrix();
    shaders[0].set_uniform("view_u", view_u);

    glm::mat4 model_u = glm::mat4(1.0f);
    model_u = glm::translate(model_u, glm::vec3(0.0f, 0.0f, 0.0f));
    model_u = glm::scale(model_u, glm::vec3(1.0f, 1.0f, 1.0f));
    shaders[0].set_uniform("model_u", model_u);

    model.draw(shaders[0]);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
