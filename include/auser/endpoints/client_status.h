#pragma once

#include <string>

#include "boost/url/url_view.hpp"

#include "auser/connection.h"

namespace auser {

struct client_status {
  std::string operator()(std::string_view) const;

  connection const& conn_;
};

}  // namespace auser