#pragma once

#include <chrono>

namespace auser {

using time_t = std::chrono::time_point<std::chrono::system_clock,
                                       std::chrono::nanoseconds>;

time_t now();

std::string timestamp(time_t);

time_t parse_timestamp(std::string const&);

}  // namespace auser