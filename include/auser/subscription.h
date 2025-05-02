#pragma once

#include "boost/asio/awaitable.hpp"
#include "boost/asio/io_context.hpp"

#include "auser/config.h"
#include "auser/connection.h"

namespace auser {

void subscription(boost::asio::io_context&, config const&, connection&);

void shutdown(boost::asio::io_context&, config const&, connection&);

}  // namespace auser