#include "auser/subscription.h"

#include <iostream>

#include "boost/asio/co_spawn.hpp"
#include "boost/asio/detached.hpp"
#include "boost/asio/experimental/parallel_group.hpp"
#include "boost/asio/redirect_error.hpp"
#include "boost/asio/steady_timer.hpp"
#include "boost/url.hpp"

#include "fmt/printf.h"

#include "utl/to_vec.h"

#include "auser/config.h"
#include "auser/connection.h"
#include "auser/http.h"
#include "auser/xml.h"

namespace auser {

pugi::xml_node add_sub_req_node(pugi::xml_node& node,
                                std::string const& sender) {
  auto sub_req_node = node.append_child("AboAnfrage");
  sub_req_node.append_attribute("Sender") = sender.c_str();
  sub_req_node.append_attribute("Zst") = timestamp(now()).c_str();
  return sub_req_node;
}

std::string unsubscribe_body(connection const& conn) {
  auto doc = make_xml_doc();
  add_sub_req_node(doc, conn.cfg_.client_name_)
      .append_child("AboLoeschenAlle")
      .append_child(pugi::node_pcdata)
      .set_value("true");
  return xml_to_str(doc);
}

std::string subscribe_body(config const& cfg, connection const& conn) {
  auto doc = make_xml_doc();
  auto sub_req_node = add_sub_req_node(doc, conn.cfg_.client_name_);
  auto sub_node = sub_req_node.append_child("AboAUS");
  sub_node.append_attribute("AboID") = std::to_string(conn.id_).data();
  sub_node.append_attribute("VerfallZst") =
      timestamp(now() + std::chrono::seconds{cfg.subscription_duration_})
          .c_str();
  auto hysteresis_node = sub_node.append_child("Hysterese");
  hysteresis_node.append_child(pugi::node_pcdata)
      .set_value(std::to_string(conn.cfg_.hysteresis_).c_str());
  auto lookahead_node = sub_node.append_child("Vorschauzeit");
  lookahead_node.append_child(pugi::node_pcdata)
      .set_value(std::to_string(std::chrono::round<std::chrono::minutes>(
                                    std::chrono::seconds{cfg.lookahead_})
                                    .count())
                     .c_str());
  return xml_to_str(doc);
}

boost::asio::awaitable<void> unsubscribe(boost::asio::io_context& ioc,
                                         config const& cfg,
                                         std::vector<connection>& conns) {
  co_await boost::asio::co_spawn(
      ioc,
      [&cfg, &conns]() -> boost::asio::awaitable<void> {
        auto executor = co_await boost::asio::this_coro::executor;
        auto awaitables = utl::to_vec(conns, [&](auto&& conn) {
          return boost::asio::co_spawn(
              executor,
              [&cfg, &conn]() -> boost::asio::awaitable<void> {
                conn.stop();
                try {
                  auto const res = co_await http_POST(
                      boost::urls::url{conn.subscription_addr_}, kHeaders,
                      unsubscribe_body(conn),
                      std::chrono::seconds{cfg.timeout_});
                  if (res.result_int() != 200U) {
                    fmt::println("[unsubscribe] failed: {}",
                                 get_http_body(res));
                  } else {
                    fmt::println("[unsubscribe] success: {} @ {}",
                                 conn.cfg_.server_name_,
                                 conn.subscription_addr_);
                  }
                } catch (std::exception const& e) {
                  fmt::println("[unsubscribe] exception: {}", e.what());
                }
              },
              boost::asio::deferred);
        });
        co_await boost::asio::experimental::make_parallel_group(awaitables)
            .async_wait(boost::asio::experimental::wait_for_all(),
                        boost::asio::use_awaitable);
      },
      boost::asio::use_awaitable);
}

boost::asio::awaitable<void> subscribe(boost::asio::io_context& ioc,
                                       config const& cfg,
                                       std::vector<connection>& conns) {
  co_await boost::asio::co_spawn(
      ioc,
      [&cfg, &conns]() -> boost::asio::awaitable<void> {
        auto executor = co_await boost::asio::this_coro::executor;
        auto awaitables = utl::to_vec(conns, [&](auto&& conn) {
          return boost::asio::co_spawn(
              executor,
              [&cfg, &conn]() -> boost::asio::awaitable<void> {
                conn.start();
                try {
                  auto const res = co_await http_POST(
                      boost::urls::url{conn.subscription_addr_}, kHeaders,
                      subscribe_body(cfg, conn),
                      std::chrono::seconds{cfg.timeout_});
                  if (res.result_int() != 200U) {
                    conn.stop();
                    fmt::println("[subscribe] failed: {}", get_http_body(res));
                  } else {
                    fmt::println("[subscribe] success: {} @ {}",
                                 conn.cfg_.server_name_,
                                 conn.subscription_addr_);
                  }
                } catch (std::exception const& e) {
                  conn.stop();
                  fmt::println("[subscribe] exception: {}", e.what());
                }
              },
              boost::asio::deferred);
        });
        co_await boost::asio::experimental::make_parallel_group(awaitables)
            .async_wait(boost::asio::experimental::wait_for_all(),
                        boost::asio::use_awaitable);
      },
      boost::asio::use_awaitable);
}

void subscription(boost::asio::io_context& ioc,
                  config const& cfg,
                  std::vector<connection>& conns) {
  boost::asio::co_spawn(
      ioc,
      [&cfg, &conns, &ioc]() -> boost::asio::awaitable<void> {
        auto executor = co_await boost::asio::this_coro::executor;
        auto timer = boost::asio::steady_timer{executor};
        auto ec = boost::system::error_code{};
        while (true) {
          auto const start = std::chrono::steady_clock::now();

          co_await unsubscribe(ioc, cfg, conns);
          co_await subscribe(ioc, cfg, conns);

          timer.expires_at(start +
                           std::chrono::seconds{cfg.subscription_duration_});
          co_await timer.async_wait(
              boost::asio::redirect_error(boost::asio::use_awaitable, ec));
          if (ec == boost::asio::error::operation_aborted) {
            co_return;
          }
        }
      },
      boost::asio::detached);
}

void shutdown(boost::asio::io_context& ioc,
              config const& cfg,
              std::vector<connection>& conns) {
  boost::asio::co_spawn(
      ioc,
      [&cfg, &conns, &ioc]() -> boost::asio::awaitable<void> {
        co_await unsubscribe(ioc, cfg, conns);
      },
      boost::asio::detached);
}

}  // namespace auser