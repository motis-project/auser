#include "auser/get_upstream.h"
#include <fstream>

#include "boost/asio/co_spawn.hpp"
#include "boost/asio/detached.hpp"
#include "boost/asio/experimental/parallel_group.hpp"
#include "boost/asio/redirect_error.hpp"
#include "boost/asio/steady_timer.hpp"

#include "utl/helpers/algorithm.h"
#include "utl/to_vec.h"

#include "fmt/printf.h"

#include "auser/http.h"
#include "auser/xml.h"

namespace auser {

bool but_wait_there_is_more(pugi::xml_document const& doc) {
  auto const found_more = doc.select_node("//WeitereDaten");
  return found_more && found_more.node().text().as_bool();
}

history_t cleanup_copy(history_t const& u, config const& cfg) {
  auto const old = (now() - std::chrono::seconds{cfg.subscription_duration_})
                       .time_since_epoch()
                       .count();

  auto copy = history_t{};
  for (auto const& [k, v] : u) {
    if (k < old) {
      continue;
    }

    copy[k] = make_xml_doc();
    for (auto const& c : v) {
      copy[k].append_copy(c);
    }
  }

  return copy;
}

void get_upstream(boost::asio::io_context& ioc,
                  config const& cfg,
                  std::vector<connection>& conns,
                  std::shared_ptr<std::unique_ptr<history_t>>& history) {
  boost::asio::co_spawn(
      ioc,
      [&cfg, &conns, &history]() -> boost::asio::awaitable<void> {
        auto executor = co_await boost::asio::this_coro::executor;
        auto timer = boost::asio::steady_timer{executor};
        auto ec = boost::system::error_code{};

        while (true) {
          auto const start = std::chrono::steady_clock::now();

          auto new_history =
              std::make_unique<history_t>(cleanup_copy(**history, cfg));

          for (auto& conn : conns) {
            conn.needs_update_ = true;
          }
          while (utl::any_of(
              conns, [](auto const& conn) { return conn.needs_update_; })) {
            auto awaitables = utl::to_vec(conns, [&](connection& conn) {
              return boost::asio::co_spawn(
                  executor, [&]() -> boost::asio::awaitable<void> {
                    if (!conn.needs_update_) {
                      co_return;
                    }

                    try {
                      auto const res = co_await http_POST(
                          boost::urls::url{conn.get_upstream_data_addr_},
                          kHeaders, conn.make_get_upstream_req(),
                          std::chrono::seconds{conn.cfg_.timeout_});

                      auto k = now().time_since_epoch().count();
                      while (new_history->contains(k)) {
                        ++k;
                      }
                      new_history->try_emplace(k, parse(get_http_body(res)));

                      conn.needs_update_ =
                          but_wait_there_is_more(new_history->at(k));

                      new_history->at(k).save_file(std::to_string(k).c_str());
                    } catch (std::exception const& e) {
                      fmt::println("[get_upstream] catch: {}", e.what());
                    }
                  });
            });

            auto [idx, exceptions] =
                co_await boost::asio::experimental::make_parallel_group(
                    awaitables)
                    .async_wait(boost::asio::experimental::wait_for_all(),
                                boost::asio::use_awaitable);
          }

          history = std::make_shared<std::unique_ptr<history_t>>(
              std::move(new_history));
          fmt::println("[get_upstream] History entries after get_upstream:{}",
                       [&]() {
                         auto ss = std::stringstream{};
                         for (auto const& [k, e] : **history) {
                           ss << "\n" << k;
                         }
                         return ss.str();
                       }());

          timer.expires_at(start + std::chrono::seconds{cfg.update_interval_});
          co_await timer.async_wait(
              boost::asio::redirect_error(boost::asio::use_awaitable, ec));
          if (ec == boost::asio::error::operation_aborted) {
            co_return;
          }
        }
      },
      boost::asio::detached);
}

}  // namespace auser