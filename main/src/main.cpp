#include <array>
#include <iostream>
#include <optional>
#include <vector>

#include <common.hpp>
#include <program_options.hpp>

namespace {

struct triangle_t {
  std::array<gl::vec3_t, 3> positions;
  std::array<gl::vec4_t, 3> colors;
};

triangle_t triangle1 = {.positions = {{{1.0f, 1.0f, 0.0f},
                                       {1.0f, -1.0f, 0.0f},
                                       {-1.0f, -1.0f, 0.0f}}},
                        .colors = {{{0.0f, 1.0f, 0.0f, 1.0f},
                                    {1.0f, 0.0f, 0.0f, 1.0f},
                                    {0.0f, 0.0f, 1.0f, 1.0f}}}};

triangle_t triangle2 = {.positions = {{{1.0f, 1.0f, 0.0f},
                                       {-1.0f, 1.0f, 0.0f},
                                       {-1.0f, -1.0f, 0.0f}}},
                        .colors = {{{0.0f, 1.0f, 0.0f, 1.0f},
                                    {1.0f, 0.0f, 0.0f, 1.0f},
                                    {0.0f, 0.0f, 1.0f, 1.0f}}}};

std::vector<gl::shader_t> shaders;
std::vector<gl::mesh_t> meshs;

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
  for (int i = 0; i < 2; i++) {
    shaders.push_back(gl::shader_t(opts.vert.value(), opts.frag.value(), true));
  }

  // build shaders
  for (auto &shader : shaders) {
    auto res = shader.build();
    if (res.err != std::nullopt) {
      LOG_ERR(res.err.value());
      return 1;
    }
  }

  // create meshs
  for (const auto &shader : shaders) {
    meshs.push_back({shader});
  }

  // set buffers
  meshs[0].use_vbo(triangle1);
  meshs[1].use_vbo(triangle2);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    for (const auto &mesh : meshs) {
      auto res = mesh.draw();
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
