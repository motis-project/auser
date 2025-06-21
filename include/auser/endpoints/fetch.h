#pragma once

#include <map>

#include "net/web_server/query_router.h"

#include "auser/time.h"
#include "auser/types.h"

namespace auser {

struct fetch {
  net::reply operator()(net::route_request const&, bool) const;

  std::shared_ptr<history_t> const& history_;
};

}  // namespace auser