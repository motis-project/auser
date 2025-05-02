#pragma once

#include <string>
#include <string_view>

namespace auser {

struct data_ready {
  std::string operator()(std::string_view) const;
};

}  // namespace auser