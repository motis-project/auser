#pragma once

#include <chrono>

namespace auser {

using time = std::chrono::time_point<std::chrono::system_clock>;

time now();

std::string timestamp(time);

time parse_timestamp(std::string const&);

}  // namespace auser