#pragma once

#include <string>

#include "boost/url/url_view.hpp"

#include "vdvauser/connection.h"

namespace vdvauser {

struct client_status {
  std::string operator()(std::string_view) const;

  connection const& vdvaus_;
};

}  // namespace vdvauser