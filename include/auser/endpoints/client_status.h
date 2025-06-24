#pragma once

#include "net/web_server/query_router.h"

#include "auser/connection.h"

namespace auser {

struct client_status {
  net::reply operator()(net::route_request const&, bool) const;

  connection const& conn_;
};

}  // namespace auser