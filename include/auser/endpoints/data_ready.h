#pragma once

#include "net/web_server/query_router.h"

namespace auser {

struct data_ready {
  net::reply operator()(net::route_request const&, bool) const;
};

}  // namespace auser