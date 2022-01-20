#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <common.hpp>
#include <shader.hpp>

#define LOG_LEVEL LOG_LEVEL_INFO
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace {

namespace b_opt = boost::program_options;
namespace b_fs = boost::filesystem;

void parse_opts(int argc, char *argv[]) {
  b_opt::options_description desc("all options");
  desc.add_options()("help,h", "")("port", b_opt::value<int>(), "ip port");

  b_opt::variables_map vm;
  b_opt::store(b_opt::parse_command_line(argc, argv, desc), vm);
  b_opt::notify(vm);

  if (vm.count("help")) {
    std::cout << "help ?" << std::endl;
    exit(0);
  }
}

GLuint generateObjectBuffer(GLfloat vertices[], GLfloat colors[]) {
  GLuint numVertices = 3;
  // Genderate 1 generic buffer object, called VBO
  GLuint VBO;
  glGenBuffers(1, &VBO);
  // In OpenGL, we bind (make active) the handle to a target name and then
  // execute commands on that target Buffer will contain an array of vertices
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // After binding, we now fill our object with data, everything in "Vertices"
  // goes to the GPU
  glBufferData(GL_ARRAY_BUFFER, numVertices * 7 * sizeof(GLfloat), NULL,
               GL_STATIC_DRAW);
  // if you have more data besides vertices (e.g., vertex colours or normals),
  // use glBufferSubData to tell the buffer when the vertices array ends and
  // when the colors start
  glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * 3 * sizeof(GLfloat),
                  vertices);
  glBufferSubData(GL_ARRAY_BUFFER, numVertices * 3 * sizeof(GLfloat),
                  numVertices * 4 * sizeof(GLfloat), colors);
  return VBO;
}

void linkCurrentBuffertoShader(GLuint shaderProgramID) {
  GLuint numVertices = 3;
  // find the location of the variables that we will be using in the shader
  // program
  GLuint positionID = glGetAttribLocation(shaderProgramID, "vPosition");
  GLuint colorID = glGetAttribLocation(shaderProgramID, "vColor");
  // Have to enable this
  glEnableVertexAttribArray(positionID);
  // Tell it where to find the position data in the currently active buffer (at
  // index positionID)
  glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);
  // Similarly, for the color data.
  glEnableVertexAttribArray(colorID);
  glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0,
                        BUFFER_OFFSET(numVertices * 3 * sizeof(GLfloat)));
}

} // namespace

int main(int argc, char **argv) {
  parse_opts(argc, argv);

  opengl::init_config_t cfg = {.argc = argc,
                               .argv = argv,
                               .mw_title = "Hello Triangle",
                               .w_size = {800, 600},
                               .display_mode = GLUT_3_2_CORE_PROFILE |
                                               GLUT_DOUBLE | GLUT_RGB};
  auto res = opengl::init(cfg);
  if (res.err != std::nullopt) {
    LOG_ERR(res.err.value());
    return 1;
  }

  std::string file = "./shader";
  std::vector<std::string> glsl_files;
  for (b_fs::directory_iterator iter(file); iter != b_fs::end(iter); iter++) {
    if (b_fs::is_regular_file(*iter)) {
      glsl_files.push_back(iter->path().string());
    }
  }

  auto res_1 = shader::create_program(glsl_files);
  if (res_1.err != std::nullopt) {
    LOG_ERR(res_1.err.value());
    return 1;
  }
  GLuint shader_pid = res_1.result;

  glUseProgram(shader_pid);

  GLfloat vertices[] = {-1.0f, -1.0f, 0.0f, 1.0f, -1.0f,
                        0.0f,  0.0f,  1.0f, 0.0f};

  GLfloat colors[] = {0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f};

  generateObjectBuffer(vertices, colors);
  linkCurrentBuffertoShader(shader_pid);

  glutMainLoop();
  return 0;
}