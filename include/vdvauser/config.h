#pragma once

#include "net/http/client/url.h"

namespace vdvauser {

    struct config {
        unsigned n_threads_{2};
        std::string data_dir_{"data"};
        std::string client_name_{"client"};
        std::string client_ip_{"0.0.0.0"};
        std::string client_port_{"8080"};
        std::string server_name_;
        std::string server_addr_;
        std::uint32_t timeout_{30U}; // s
        std::uint32_t hysteresis_{30U}; // s
        std::uint32_t lookahead_{90000}; // s
        std::uint32_t subscription_duration_{3600U}; //s
    };

}  // namespace vdvauser