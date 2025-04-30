#include <iostream>

#include "boost/program_options.hpp"

#include "vdvausproxy/config.h"

namespace bpo = boost::program_options;

int main(int argc, char* argv[]) {
    auto cfg = vdvausproxy::config{};
    bpo::options_description desc("Allowed options");
    desc.add_options()("help,h", "produce this help message")  //
            ("client_name", bpo::value(&cfg.client_name_)->required())  //
            ("client_ip", bpo::value(&cfg.client_ip_)->required())  //
            ("client_port", bpo::value(&cfg.client_port_)->required())  //
            ("server_name", bpo::value(&cfg.server_name_)->required())  //
            ("server_addr", bpo::value(&cfg.server_addr_)->required());
    bpo::variables_map vm;
    bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(), vm);
    if (vm.count("help") != 0U) {
        std::cout << desc << "\n";
        return 0;
    }
    bpo::notify(vm);
    cfg.derive_endpoints();



}