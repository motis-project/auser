#pragma once

#include <map>
#include <string>
#include <string_view>

#include "vdvauser/config.h"
#include "vdvauser/time.h"

namespace vdvauser {

auto const kHeaders = std::map<std::string, std::string>{
{"Content-Type", "text/xml"}, {"Accept", "text/xml"}};

struct connection {
    explicit connection(vdvauser::config const&);
    void start();
    void stop();
    std::string make_fetch_req();

    vdvauser::config const& cfg_;
    std::string client_status_path_;
    std::string data_ready_path_;
    std::string server_status_addr_;
    std::string subscription_addr_;
    std::string fetch_data_addr_;
    std::atomic<std::chrono::seconds::rep> id_{0};
    std::atomic<time> start_{time::min()};
};

} // namespace vdvauser
