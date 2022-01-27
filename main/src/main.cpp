#include <array>
#include <iostream>
#include <optional>

#include <boost/pfr.hpp>
#include <boost/program_options.hpp>

#include <common.hpp>

#define LOG_LEVEL LOG_LEVEL_INFO

namespace {

namespace b_pfr = boost::pfr;
namespace b_opt = boost::program_options;

std::optional<std::string> vert = std::nullopt;
std::optional<std::string> frag = std::nullopt;

struct triangle_t {
  std::array<gl::vec3_t, 3> positions;
  std::array<gl::vec4_t, 3> colors;
};

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

  gl::init_config_t cfg = {.argc = argc,
                               .argv = argv,
                               .mw_title = "Hello Triangle",
                               .win_size = {800, 600},
                               .display_mode = GLUT_3_2_CORE_PROFILE |
                                               GLUT_DOUBLE | GLUT_RGB};
  auto res_0 = gl::init(cfg);
  if (res_0.err != std::nullopt) {
    LOG_ERR(res_0.err.value());
    return 1;
  }

  gl::program_t program({vert.value(), frag.value(), true});
  auto res_1 = program.build();
  if (res_1.err != std::nullopt) {
    LOG_ERR(res_1.err.value());
    return 1;
  }

  triangle_t triangle = {.positions = {{{1.0f, 1.0f, 0.0f},
                                        {1.0f, -1.0f, 0.0f},
                                        {-1.0f, -1.0f, 0.0f}}},
                         .colors = {{{0.0f, 1.0f, 0.0f, 1.0f},
                                     {1.0f, 0.0f, 0.0f, 1.0f},
                                     {0.0f, 0.0f, 1.0f, 1.0f}}}};

  program.create_vbo(triangle);
  program.use_vbo(triangle, {"vPosition", "vColor"});
  program.use();

  glutMainLoop();

  return 0;
}
