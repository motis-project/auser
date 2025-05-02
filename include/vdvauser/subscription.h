#pragma once

#include "boost/asio/awaitable.hpp"
#include "boost/asio/io_context.hpp"

#include "vdvauser/config.h"
#include "vdvauser/connection.h"

namespace vdvauser {

void subscription(boost::asio::io_context&, config const&, connection&);

void shutdown(boost::asio::io_context&, config const&, connection&);

}  // namespace vdvauser