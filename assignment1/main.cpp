#include "figine/figine.hpp"
#include "global.hpp"

using namespace cs7gv3::ass1;

void process_input(GLFWwindow *window, float delta_time);

int main() {
  figine::global::init();

  GLFWwindow *window = figine::global::win_mgr::create_window(
      800, 600, "cs7gv3 - assignment 1", NULL, NULL);

  init();

  figine::imnotgui::init(window);
  figine::imnotgui::register_window(&phong_console);
  figine::imnotgui::register_window(&gooch_console);
  figine::imnotgui::register_window(&cook_torrance_console);

  camera.lock({0, 0, 0});
  float last_time = 0;
  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float current_time = glfwGetTime();
    float delta_time = current_time - last_time;
    last_time = current_time;

    process_input(window, delta_time);

    teapot[0].loop(phong_shader);
    // teapot[1].loop(gooch_shader);
    // teapot[2].loop(cook_torrance_shader);

    figine::imnotgui::render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}

void process_input(GLFWwindow *window, float delta_time) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera.process_keyboard(figine::core::camera_movement_t::FORWARD,
                            delta_time);
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera.process_keyboard(figine::core::camera_movement_t::BACKWARD,
                            delta_time);
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera.process_keyboard(figine::core::camera_movement_t::LEFT, delta_time);
  }

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera.process_keyboard(figine::core::camera_movement_t::RIGHT, delta_time);
  }
}