#pragma once

#include <chrono>

#include "auser/types.h"

namespace auser {

time_t now();

std::string timestamp(time_t);

time_t parse_timestamp(std::string const&);

}  // namespace auser