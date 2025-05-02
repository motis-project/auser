#pragma once

#include <map>
#include <string>
#include <string_view>

#include "auser/config.h"
#include "auser/time.h"

namespace auser {

auto const kHeaders = std::map<std::string, std::string>{
    {"Content-Type", "text/xml"}, {"Accept", "text/xml"}};

struct connection {
  explicit connection(auser::config const&);
  connection(connection&&) noexcept;

  void start();
  void stop();
  std::string make_fetch_req() const;

  auser::config const& cfg_;
  std::string client_status_path_;
  std::string data_ready_path_;
  std::string server_status_addr_;
  std::string subscription_addr_;
  std::string fetch_data_addr_;
  std::atomic<std::chrono::seconds::rep> id_{0};
  std::atomic<time> start_{time::min()};
  bool needs_update_;
};

}  // namespace auser
