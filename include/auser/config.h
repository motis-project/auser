#pragma once

#include <cstdint>
#include <atomic>

#include "net/http/client/url.h"

namespace auser {

struct config {
  unsigned n_threads_{2};
  std::string client_name_{"client"};
  std::string client_ip_{"0.0.0.0"};
  std::string client_port_{"8080"};
  std::string server_name_;
  std::string server_addr_;
  std::uint32_t timeout_{30U};  // s
  std::uint32_t hysteresis_{30U};  // s
  std::uint32_t lookahead_{12 * 3600U};  // s
  std::uint32_t subscription_duration_{300U};  // s
  std::uint32_t update_interval_{15U};  // s
};

}  // namespace auser