#include <iostream>

#include "boost/program_options.hpp"
#include "boost/asio/io_context.hpp"

#include "net/run.h"
#include "net/web_server/web_server.h"
#include "net/stop_handler.h"

#include "utl/enumerate.h"
#include "utl/set_thread_name.h"

#include "vdvauser/config.h"
#include "vdvauser/connection.h"
#include "vdvauser/subscription.h"
#include "vdvauser/scheduler/runner.h"
#include "vdvauser/endpoints/client_status.h"
#include "vdvauser/endpoints/data_ready.h"

namespace bpo = boost::program_options;

int main(int argc, char *argv[]) {
    auto cfg = vdvauser::config{};
    bpo::options_description desc("Allowed options");
    desc.add_options()("help,h", "produce this help message") //
            ("n_threads", bpo::value(&cfg.n_threads_)->default_value(cfg.n_threads_))  //
            ("client_name", bpo::value(&cfg.client_name_)->default_value(cfg.client_name_))  //
            ("client_ip", bpo::value(&cfg.client_ip_)->default_value(cfg.client_ip_))  //
            ("client_port", bpo::value(&cfg.client_port_)->default_value(cfg.client_port_))  //
            ("server_name", bpo::value(&cfg.server_name_)->required())  //
            ("server_addr", bpo::value(&cfg.server_addr_)->required());
    bpo::variables_map vm;
    bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(), vm);
    if (vm.count("help") != 0U) {
        std::cout << desc << "\n";
        return 0;
    }
    bpo::notify(vm);

    auto ioc = boost::asio::io_context{};
    auto s = net::web_server{ioc};
    auto r = vdvauser::runner{cfg.n_threads_, 1024U};
    auto qr = net::query_router{net::fiber_exec{ioc, r.ch_}};
    qr.add_header("Server", "vdvauser");
    auto con = vdvauser::connection{cfg};
    qr.route("POST", con.client_status_path_, vdvauser::client_status{con});
    qr.route("POST", con.data_ready_path_, vdvauser::data_ready{});
    fmt::println("setup client status path: {}", con.client_status_path_);
    fmt::println("setup data ready path: {}", con.data_ready_path_);

    qr.enable_cors();
    s.set_timeout(std::chrono::minutes{5});
    s.on_http_request(std::move(qr));

    auto ec = boost::system::error_code{};
    s.init(cfg.client_ip_, cfg.client_port_, ec);
    s.run();

    if (ec) {
        std::cerr << "error: " << ec << "\n";
        return 1;
    }

    std::cout << "listening on " << cfg.client_ip_ << ":" << cfg.client_port_ << "\n";

    auto vdvaus_subscription_ioc = std::make_unique<boost::asio::io_context>();
    auto vdvaus_subscription_thread = std::make_unique<std::thread>([&]() {
        utl::set_current_thread_name("motis vdvaus subscription");
        vdvauser::subscription(*vdvaus_subscription_ioc, cfg, con);
        vdvaus_subscription_ioc->run();
    });


    auto threads = std::vector<std::thread>{cfg.n_threads_};
    for (auto [i, t]: utl::enumerate(threads)) {
        t = std::thread{r.run_fn()};
        utl::set_thread_name(t, fmt::format("vdvaus worker {}", i));
    }

    auto const stop = net::stop_handler(ioc, [&]() {
        fmt::println("vdvauser shutdown");
        r.ch_.close();
        s.stop();
        ioc.stop();

        if (vdvaus_subscription_ioc != nullptr) {
            vdvaus_subscription_ioc->stop();
            auto vdvaus_unsubscribe_ioc = std::make_unique<boost::asio::io_context>();
            auto vdvaus_unsubscribe_thread = std::thread{[&]() {
                utl::set_current_thread_name("vdvaus unsubscribe");
                vdvauser::shutdown(*vdvaus_unsubscribe_ioc, cfg, con);
                vdvaus_unsubscribe_ioc->run();
            }};
            vdvaus_unsubscribe_thread.join();
            vdvaus_unsubscribe_ioc->stop();
        }
    });

    net::run(ioc)();
}