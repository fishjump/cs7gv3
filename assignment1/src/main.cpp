#include <array>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

#include <cs7gvx_utils.hpp>
#include <global.hpp>
#include <io.hpp>
#include <teapot_model.hpp>

int main(int argc, char **argv) {
  cs7gvx_utils::gl::init();
  defer(glfwTerminate());

  GLFWwindow *window = glfwCreateWindow(cs7gv3::SCR_WIDTH, cs7gv3::SCR_HEIGHT,
                                        "cs7gv3", NULL, NULL);
  if (window == NULL) {
    LOG_ERR("Failed to create GLFW window");
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, cs7gv3::framebuffer_size_callback);
  glfwSetCursorPosCallback(window, cs7gv3::mouse_callback);
  glfwSetScrollCallback(window, cs7gv3::scroll_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

  cs7gvx_utils::imnotgui::init(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    LOG_ERR("Failed to initialize GLAD");
    return -1;
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  cs7gv3::global::init();

  cs7gvx_utils::imnotgui::demo_window_t demo_win;
  cs7gvx_utils::imnotgui::register_window(&demo_win);

  cs7gv3::global::camera.lock({0, 0, 0});

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cs7gv3::process_input(window);

    cs7gv3::global::teapot1.loop();
    cs7gv3::global::teapot2.loop();

    cs7gvx_utils::imnotgui::render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}
