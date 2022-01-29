#ifndef CS7GV3_PROGRAM_OPTIONS_HPP
#define CS7GV3_PROGRAM_OPTIONS_HPP

#include <optional>

#include <boost/program_options.hpp>

namespace cs7gv3 {

struct program_options_t {
  std::optional<std::string> vert = std::nullopt;
  std::optional<std::string> frag = std::nullopt;
};

const program_options_t &parse_opts(int argc, char *argv[]);

} // namespace cs7gv3

#endif // CS7GV3_PROGRAM_OPTIONS_HPP