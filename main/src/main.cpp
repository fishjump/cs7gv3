#include <iostream>
#include <optional>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <common.hpp>
#include <program.hpp>

#define LOG_LEVEL LOG_LEVEL_INFO

namespace {

namespace b_opt = boost::program_options;
namespace b_fs = boost::filesystem;

std::optional<std::string> vert = std::nullopt;
std::optional<std::string> frag = std::nullopt;

void parse_opts(int argc, char *argv[]) {
  b_opt::options_description desc("all options");
  desc.add_options()("help,h", "")("vert,v", b_opt::value<std::string>())(
      "frag,f", b_opt::value<std::string>());

  b_opt::variables_map vm;
  b_opt::store(b_opt::parse_command_line(argc, argv, desc), vm);
  b_opt::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    exit(0);
  }

  if (vm.count("vert")) {
    vert = vm["vert"].as<std::string>();
  }

  if (vm.count("frag")) {
    frag = vm["frag"].as<std::string>();
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
  auto res_0 = opengl::init(cfg);
  if (res_0.err != std::nullopt) {
    LOG_ERR(res_0.err.value());
    return 1;
  }

  cs7gv3::program_t program({vert.value_or(""), frag.value_or(""), true});
  auto res_1 = program.build();
  if (res_1.err != std::nullopt) {
    LOG_ERR(res_1.err.value());
    return 1;
  }

  opengl::triangle_t triangle = {.positions = {{1.0f, 1.0f, 0.0f},
                                               {1.0f, -1.0f, 0.0f},
                                               {-1.0f, -1.0f, 0.0f}},
                                 .colors = {{0.0f, 1.0f, 0.0f, 1.0f},
                                            {1.0f, 0.0f, 0.0f, 1.0f},
                                            {0.0f, 0.0f, 1.0f, 1.0f}}};
  program.create_vbo(triangle);
  program.use_vbo(triangle);
  program.use();

  glutMainLoop();
  return 0;
}
