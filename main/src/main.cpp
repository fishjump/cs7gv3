#include <array>
#include <iostream>
#include <optional>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <common.hpp>
#include <gl.hpp>
#include <io.hpp>
#include <program_options.hpp>

namespace {

glm::vec3 light_pos = {.0f, 2.0f, 2.0f};
float last_frame = 0.0f;

} // namespace

int main(int argc, char **argv) {
  gl::init();
  defer(glfwTerminate());

  GLFWwindow *window = glfwCreateWindow(cs7gv3::SCR_WIDTH, cs7gv3::SCR_HEIGHT,
                                        "cs7gv3", NULL, NULL);
  if (window == NULL) {
    LOG_ERR("Failed to create GLFW window");
    return -1;
  }

  glfwMakeContextCurrent(window);
  // glfwSetFramebufferSizeCallback(window, cs7gv3::framebuffer_size_callback);
  // glfwSetCursorPosCallback(window, cs7gv3::mouse_callback);
  // glfwSetScrollCallback(window, cs7gv3::scroll_callback);
  // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    LOG_ERR("Failed to initialize GLAD");
    return -1;
  }

  glEnable(GL_DEPTH_TEST);

  // create shaders
  gl::shader_t phong_shader("shader/base.vs", "shader/phong.fs");
  gl::shader_t light_cube_shader("shader/light_cube.vs",
                                 "shader/light_cube.fs");

  gl::model_t lamp("model/light_cude.obj", [&](gl::model_t &self) {
    self.translate(light_pos).scale(glm::vec3(0.2f));
  });

  gl::model_t teapot1(
      "model/teapot.obj",
      [&](gl::model_t &self) {
        self.translate({-1.5, 0, 0});
      },
      [&](gl::model_t &self) {
        self.rotate(1, {0, 1, 0});
      });

  gl::model_t teapot2(
      "model/teapot.obj",
      [&](gl::model_t &self) {
        self.translate({1.5, 0, 0});
      },
      [&](gl::model_t &self) {
        self.rotate(1, {0, 1, 0});
      });

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float current_frame = (float)glfwGetTime();
    cs7gv3::delta_time() = current_frame - last_frame;
    last_frame = current_frame;

    // cs7gv3::process_input(window);

    teapot1.loop();
    teapot2.loop();
    lamp.loop();

    phong_shader.use();
    phong_shader.set_uniform("view_pos", cs7gv3::camera().position());

    // light properties
    glm::vec3 light_color(1);
    glm::vec3 diffuse_color = light_color * glm::vec3(0.5f);
    glm::vec3 ambient_color = diffuse_color * glm::vec3(2.0f);
    phong_shader.set_uniform("light.position", light_pos);
    phong_shader.set_uniform("light.ambient_color", ambient_color);
    phong_shader.set_uniform("light.diffuse_color", diffuse_color);
    phong_shader.set_uniform("light.specular_color", glm::vec3(1.0f));

    // material properties
    phong_shader.set_uniform("material.ambient_color", glm::vec3(0));
    phong_shader.set_uniform("material.diffuse_color", glm::vec3(1.0));
    phong_shader.set_uniform("material.specular_color", glm::vec3(0.5));
    phong_shader.set_uniform("material.shininess", 16.0f);

    // view/projection transformations
    glm::mat4 projection = glm::perspective(
        glm::radians(cs7gv3::camera().zoom()),
        (float)cs7gv3::SCR_WIDTH / (float)cs7gv3::SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = cs7gv3::camera().view_matrix();
    phong_shader.set_uniform("projection_uni", projection);
    phong_shader.set_uniform("view_uni", view);

    // world transformation
    phong_shader.set_uniform("model_uni", teapot1.position());
    teapot1.draw(phong_shader);

    phong_shader.set_uniform("model_uni", teapot2.position());
    teapot2.draw(phong_shader);

    // also draw the lamp object
    light_cube_shader.use();
    light_cube_shader.set_uniform("projection_uni", projection);
    light_cube_shader.set_uniform("view_uni", view);

    light_cube_shader.set_uniform("model_uni", lamp.position());

    lamp.draw(light_cube_shader);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}
