#include <iostream>

#include "boost/program_options.hpp"
#include "boost/asio/io_context.hpp"

#include "net/web_server/web_server.h"

#include "vdvauser/config.h"
#include "vdvauser/connection.h"
#include "vdvauser/scheduler/runner.h"

namespace bpo = boost::program_options;

int main(int argc, char* argv[]) {
    auto cfg = vdvauser::config{};
    bpo::options_description desc("Allowed options");
    desc.add_options()("help,h", "produce this help message") //
            ("client_name", bpo::value(&cfg.client_name_))  //
            ("client_ip", bpo::value(&cfg.client_ip_))  //
            ("client_port", bpo::value(&cfg.client_port_))  //
            ("server_name", bpo::value(&cfg.server_name_)->required())  //
            ("server_addr", bpo::value(&cfg.server_addr_)->required());
    bpo::variables_map vm;
    bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(), vm);
    if (vm.count("help") != 0U) {
        std::cout << desc << "\n";
        return 0;
    }
    bpo::notify(vm);

    auto con = vdvauser::connection{cfg};
    auto ioc = boost::asio::io_context{};
    auto s = net::web_server{ioc};
    auto r = vdvauser::runner{2,1204U};
    auto qr = net::query_router{net::fiber_exec{ioc, r.ch_}};
    qr.add_header("Server", "vdvauser");


}