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

history_t cleaned_up(history_t const& h, time_t::rep const discard_before) {

  auto copy = history_t{};
  for (auto const& [k, v] : h) {
    if (k < discard_before) {
      continue;
    }

    copy[k] = make_xml_doc();
    for (auto const& c : v) {
      copy[k].append_copy(c);
    }
  }

  return copy;
}

bool but_wait_there_is_more(pugi::xml_document const& doc) {
  auto const found_more = doc.select_node("//WeitereDaten");
  return found_more && found_more.node().text().as_bool();
}

size_t n_rides_in_msg(pugi::xml_document const& doc) {
  return doc.select_nodes("//IstFahrt").size();
}

void get_upstream(boost::asio::io_context& ioc,
                  config const& cfg,
                  std::vector<connection>& conns,
                  std::shared_ptr<history_t>& history) {
  boost::asio::co_spawn(
      ioc,
      [&cfg, &conns, &history]() -> boost::asio::awaitable<void> {
        auto executor = co_await boost::asio::this_coro::executor;
        auto timer = boost::asio::steady_timer{executor};
        auto ec = boost::system::error_code{};

        while (true) {
          auto const start = std::chrono::steady_clock::now();

          auto discard_before = std::numeric_limits<time_t::rep>::max();
          for (auto& conn : conns) {
            discard_before = std::min(discard_before, conn.prev_id_.load());
            conn.needs_update_ = true;
          }

          auto new_history = cleaned_up(*history, discard_before);
          auto const prev_last_key =
              new_history.empty() ? 0L : rbegin(new_history)->first;
          auto m = std::mutex{};
          auto n_rides_total = size_t{0U};
          while (utl::any_of(
              conns, [](auto const& conn) { return conn.needs_update_; })) {
            auto awaitables = utl::to_vec(conns, [&](connection& conn) {
              return boost::asio::co_spawn(
                  executor, [&]() -> boost::asio::awaitable<void> {
                    if (!conn.needs_update_) {
                      co_return;
                    }

                    try {
                      auto const network_start =
                          std::chrono::steady_clock::now();
                      auto const res = co_await http_POST(
                          boost::urls::url{conn.get_upstream_data_addr_},
                          kHeaders, conn.make_get_upstream_req(),
                          std::chrono::seconds{conn.cfg_.timeout_});
                      auto const network_time =
                          std::chrono::duration_cast<std::chrono::milliseconds>(
                              std::chrono::steady_clock::now() - network_start);

                      auto k = now().time_since_epoch().count();
                      auto const lock = std::lock_guard<std::mutex>{m};
                      while (new_history.contains(k)) {
                        ++k;
                      }
                      new_history.try_emplace(k, parse(get_http_body(res)));

                      auto const n_rides = n_rides_in_msg(new_history.at(k));
                      n_rides_total += n_rides;
                      conn.max_rides_per_msg_ =
                          std::max(conn.max_rides_per_msg_, n_rides);
                      auto const update_interval_exceeded =
                          std::chrono::steady_clock::now() - start >
                          std::chrono::seconds{cfg.update_interval_};
                      auto const full_message =
                          n_rides == conn.max_rides_per_msg_;
                      conn.needs_update_ =
                          !update_interval_exceeded &&
                          (but_wait_there_is_more(new_history.at(k)) ||
                           full_message);

                      fmt::println(
                          "[get_upstream] {} ({} rides), network_time: {} ms",
                          k, n_rides, network_time.count());

                      new_history.at(k).save_file(std::to_string(k).c_str());
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

          history = std::make_shared<history_t>(std::move(new_history));

          fmt::println("[get_upstream] {} --> {} ({} rides)", prev_last_key,
                       history->empty() ? 0 : rbegin(*history)->first,
                       n_rides_total);

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