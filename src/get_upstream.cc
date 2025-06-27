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

#include "auser/history.h"
#include "auser/http.h"

namespace auser {

history cleaned_up(history const& h, time_t::rep const discard_before) {
  auto copy = history{};

  for (auto const& kv : h.index_) {
    if (kv.first >= discard_before) {
      copy.index_.emplace_back(kv);
    }
  }
  if (!copy.index_.empty()) {
    copy.data_ += std::string_view{
        begin(h.data_) + static_cast<long>(copy.index_.front().second),
        end(h.data_)};
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
                  std::shared_ptr<history>& h) {
  boost::asio::co_spawn(
      ioc,
      [&cfg, &conns, &h]() -> boost::asio::awaitable<void> {
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

          auto h_new = cleaned_up(*h, discard_before);
          auto m = std::mutex{};
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

                      auto const body = get_http_body(res);
                      conn.needs_update_ = but_wait_there_is_more(parse(body));

                      auto const lock = std::lock_guard<std::mutex>{m};
                      h_new.add(body);

                      // new_history.at(k).save_file(std::to_string(k).c_str());
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

          h = std::make_shared<history>(std::move(h_new));

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