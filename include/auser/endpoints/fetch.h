#pragma once

#include <map>

#include "net/web_server/query_router.h"

#include "auser/history.h"
#include "auser/time.h"

namespace auser {

struct fetch {
  net::reply operator()(net::route_request const&, bool) const;

  std::shared_ptr<history> const& h_;
};

}  // namespace auser