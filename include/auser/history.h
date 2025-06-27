#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "auser/time.h"

namespace auser {

struct history {
  bool empty() const;
  bool contains(time_t::rep) const;
  void add(std::string_view, time_t::rep);
  void add(std::string_view);
  std::string since(time_t::rep, std::uint32_t) const;

  std::string data_;
  std::vector<std::pair<time_t::rep, std::uint32_t>> index_;
};

history copy_suffix(history const&, time_t::rep discard_before);

}  // namespace auser