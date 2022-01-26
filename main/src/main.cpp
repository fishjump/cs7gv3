#include <iostream>
#include <optional>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <common.hpp>
#include <shader.hpp>

#define LOG_LEVEL LOG_LEVEL_INFO

namespace {

namespace b_opt = boost::program_options;
namespace b_fs = boost::filesystem;

std::optional<std::string> directory = std::nullopt;

void parse_opts(int argc, char *argv[]) {
  b_opt::options_description desc("all options");
  desc.add_options()("help,h", "")("dir,d", b_opt::value<std::string>(),
                                   "shaders' directory");

  b_opt::variables_map vm;
  b_opt::store(b_opt::parse_command_line(argc, argv, desc), vm);
  b_opt::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    exit(0);
  }

  if (vm.count("dir")) {
    directory = vm["dir"].as<std::string>();
  }
}

} // namespace

int main(int argc, char **argv) {
  parse_opts(argc, argv);

  opengl::init_config_t cfg = {.argc = argc,
                               .argv = argv,
                               .mw_title = "Hello Triangle",
                               .win_size = {800, 600},
                               .display_mode = GLUT_3_2_CORE_PROFILE |
                                               GLUT_DOUBLE | GLUT_RGB};
  auto res = opengl::init(cfg);
  if (res.err != std::nullopt) {
    LOG_ERR(res.err.value());
    return 1;
  }

  std::vector<std::string> glsl_files;
  for (b_fs::directory_iterator iter(directory.value_or("."));
       iter != b_fs::end(iter); iter++) {
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

  shader::vbo_t vbo = {.vertices = {{-1.0f, -1.0f, 0.0f},
                                    {1.0f, -1.0f, 0.0f},
                                    {0.0f, 1.0f, 0.0f}},
                       .colors = {{0.0f, 1.0f, 0.0f, 1.0f},
                                  {1.0f, 0.0f, 0.0f, 1.0f},
                                  {0.0f, 0.0f, 1.0f, 1.0f}}};

  shader::vbo_gen(vbo);
  shader::link_buf(shader_pid);

  glutMainLoop();
  return 0;
}