#pragma once

#include "boost/asio/io_context.hpp"

#include "auser/config.h"
#include "auser/connection.h"

namespace auser {

bool but_wait_there_is_more(pugi::xml_document const&);

void get_upstream(boost::asio::io_context&,
                  config const&,
                  std::vector<connection>&,
                  std::shared_ptr<history_t>&);

}  // namespace auser