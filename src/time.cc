#include "auser/time.h"

#include "date/date.h"

namespace auser {

time now() { return std::chrono::system_clock::now(); }

std::string timestamp(const time t) {
  return date::format("%FT%T",
                      std::chrono::time_point_cast<std::chrono::seconds>(t));
}

time parse_timestamp(std::string const& str) {
  time parsed;
  auto ss = std::stringstream{str};
  ss >> date::parse("%FT%T", parsed);
  return parsed;
}

}  // namespace auser