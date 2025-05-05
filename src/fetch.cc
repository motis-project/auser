#include "auser/fetch.h"

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

bool but_wait_there_is_more(std::string const& s) {
  auto const doc = parse(s);
  auto const found_more = doc.select_node("//WeitereDaten");
  return found_more && found_more.node().text().as_bool();
}

void fetch(boost::asio::io_context& ioc,
           config const& cfg,
           std::vector<connection>& conns,
           std::shared_ptr<std::map<time::rep, std::string>> updates) {
  boost::asio::co_spawn(
      ioc,
      [&cfg, &conns, &updates]() -> boost::asio::awaitable<void> {
        auto executor = co_await boost::asio::this_coro::executor;
        auto timer = boost::asio::steady_timer{executor};
        auto ec = boost::system::error_code{};

        while (true) {
          auto const start = std::chrono::steady_clock::now();

          auto new_updates = *updates;

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
                          boost::urls::url{conn.fetch_data_addr_}, kHeaders,
                          conn.make_fetch_req(),
                          std::chrono::seconds{conn.cfg_.timeout_});

                      auto k = now().time_since_epoch().count();
                      while (new_updates.contains(k)) {
                        ++k;
                      }
                      new_updates[k] = get_http_body(res);

                      conn.needs_update_ =
                          but_wait_there_is_more(new_updates[k]);
                    } catch (std::exception const& e) {
                      fmt::println("fetch catch: {}", e.what());
                    }
                  });
            });

            auto [idx, exceptions] =
                co_await boost::asio::experimental::make_parallel_group(
                    awaitables)
                    .async_wait(boost::asio::experimental::wait_for_all(),
                                boost::asio::use_awaitable);
          }

          updates = std::make_shared<std::map<time::rep, std::string>>(
              std::move(new_updates));

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