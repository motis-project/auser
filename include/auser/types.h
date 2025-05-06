#pragma once

#include <map>

#include "pugixml.hpp"

namespace auser {

using time_t = std::chrono::time_point<std::chrono::system_clock>;
using history_t = std::map<time_t::rep, pugi::xml_document>;

}  // namespace auser