#include <iostream>
#include <map>

#include "boost/asio/io_context.hpp"
#include "boost/program_options.hpp"

#include "net/run.h"
#include "net/stop_handler.h"
#include "net/web_server/web_server.h"

#include "utl/enumerate.h"
#include "utl/set_thread_name.h"

#include "auser/config.h"
#include "auser/connection.h"
#include "auser/endpoints/client_status.h"
#include "auser/endpoints/data_ready.h"
#include "auser/endpoints/fetch.h"
#include "auser/get_upstream.h"
#include "auser/history.h"
#include "auser/scheduler/runner.h"
#include "auser/subscription.h"

namespace bpo = boost::program_options;
using namespace std::chrono_literals;

int main(int argc, char* argv[]) {
  auto cfg = auser::config{};
  bpo::options_description desc("Allowed options");
  desc.add_options()("help,h", "produce this help message")  //
      ("n_threads",
       bpo::value(&cfg.n_threads_)->default_value(cfg.n_threads_))  //
      ("client_name",
       bpo::value(&cfg.client_name_)->default_value(cfg.client_name_))  //
      ("client_ip",
       bpo::value(&cfg.client_ip_)->default_value(cfg.client_ip_))  //
      ("client_port",
       bpo::value(&cfg.client_port_)->default_value(cfg.client_port_))  //
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
  auto r = auser::runner{cfg.n_threads_, 1024U};
  auto qr = net::query_router{net::fiber_exec{ioc, r.ch_}};
  qr.add_header("Server", "auser");

  auto conns = std::vector<auser::connection>{};
  conns.emplace_back(cfg);
  for (auto const& conn : conns) {
    qr.route("POST", conn.client_status_path_, auser::client_status{conn});
    fmt::println("[init] {}", conn.client_status_path_);
    qr.route("POST", conn.data_ready_path_, auser::data_ready{});
    fmt::println("[init] {}", conn.data_ready_path_);
  }

  auto history = std::make_shared<auser::history>();
  qr.route("GET", "/auser/fetch", auser::fetch{history});
  fmt::println("[init] {}", "/auser/fetch");

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

  fmt::println("[init] listening on {}:{}", cfg.client_ip_, cfg.client_port_);

  auto subscription_ioc = boost::asio::io_context{};
  auto subscription_thread = std::thread{[&]() {
    utl::set_current_thread_name("VDV AUS subscription");
    auser::subscription(subscription_ioc, cfg, conns);
    subscription_ioc.run();
  }};

  auto get_upstream_ioc = boost::asio::io_context{};
  auto get_upstream_thread = std::thread{[&]() {
    utl::set_current_thread_name("VDV AUS get_upstream");
    std::this_thread::sleep_for(5s);
    get_upstream(get_upstream_ioc, cfg, conns, history);
    get_upstream_ioc.run();
  }};

  auto threads = std::vector<std::thread>{cfg.n_threads_};
  for (auto [i, t] : utl::enumerate(threads)) {
    t = std::thread{r.run_fn()};
    utl::set_thread_name(t, fmt::format("auser worker {}", i));
  }

  auto const stop = net::stop_handler(ioc, [&]() {
    fmt::println("[shutdown]");
    r.ch_.close();
    s.stop();
    ioc.stop();
    subscription_ioc.stop();

    auto unsubscribe_ioc = boost::asio::io_context{};
    auto unsubscribe_thread = std::thread{[&]() {
      utl::set_current_thread_name("vdvaus unsubscribe");
      auser::shutdown(unsubscribe_ioc, cfg, conns);
      unsubscribe_ioc.run();
    }};
    unsubscribe_thread.join();
  });

  net::run(ioc)();

  for (auto& t : threads) {
    t.join();
  }
  subscription_thread.join();

  return 0;
}