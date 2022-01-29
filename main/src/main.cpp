#include <array>
#include <iostream>
#include <optional>
#include <vector>

#include <common.hpp>
#include <gl.hpp>
#include <program_options.hpp>

namespace {

struct triangle_t {
  std::array<glm::vec3, 3> positions;
  std::array<glm::vec4, 3> colors;
};

std::vector<gl::vertex_t> triangle1 = {
    {.position = {.5f, .5f, 0.0f}, .color = {0.0f, 1.0f, 0.0f, 1.0f}},
    {.position = {.5f, -.5f, 0.0f}, .color = {1.0f, 0.0f, 0.0f, 1.0f}},
    {.position = {-.5f, -.5f, 0.0f}, .color = {0.0f, 0.0f, 1.0f, 1.0f}},
};

std::vector<gl::vertex_t> triangle2 = {
    {.position = {1.0f, 1.0f, 0.0f}, .color = {0.0f, 1.0f, 0.0f, 1.0f}},
    {.position = {-1.0f, 1.0f, 0.0f}, .color = {1.0f, 0.0f, 0.0f, 1.0f}},
    {.position = {-1.0f, -1.0f, 0.0f}, .color = {0.0f, 0.0f, 1.0f, 1.0f}},
};

std::vector<gl::shader_t> shaders;
std::vector<gl::mesh_t> meshes;

} // namespace

int main(int argc, char **argv) {
  auto opts = cs7gv3::parse_opts(argc, argv);

  gl::init();

  // glfw window creation
  // --------------------
  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  // glfwSetCursorPosCallback(window, mouse_callback);
  // glfwSetScrollCallback(window, scroll_callback);

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
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

  // create meshes
  meshes.push_back({triangle1, {0u, 1u, 2u}, {}});
  meshes.push_back({triangle2, {0u, 1u, 2u}, {}});

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    for (int i = 0; i < meshes.size(); i++) {
      auto res = meshes[i].draw(shaders[0]);
      if (res.err != std::nullopt) {
        LOG_ERR(res.err.value());
        return 1;
      }
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
