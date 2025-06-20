#pragma once

#include <map>

#include "boost/url/url_view.hpp"

#include "auser/time.h"
#include "auser/types.h"

namespace auser {

struct fetch {
  std::string operator()(boost::urls::url_view const&) const;

  std::shared_ptr<std::unique_ptr<history_t>> const& history_;
};

}  // namespace auser