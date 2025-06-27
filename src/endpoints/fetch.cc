#include "auser/endpoints/fetch.h"

#include "fmt/printf.h"

#include "auser/xml.h"

namespace auser {

constexpr auto const kDefaultBodyLimit = 8U * 1024U * 1024U;

net::reply fetch::operator()(net::route_request const& req, bool) const {
  auto const history = history_;

  auto since = time_t::rep{0};
  auto body_limit = kDefaultBodyLimit;
  for (auto const& p : req.url_.params()) {
    try {
      if (p.key == "since" && p.has_value) {
        since = std::stol(p.value);
        continue;
      }
      if (p.key == "body_limit" && p.has_value) {
        body_limit = static_cast<std::uint32_t>(std::stoul(p.value));
        continue;
      }
    } catch (std::exception const& e) {
      fmt::println("[fetch] could not parse parameter {}: {}, {}", p.key,
                   p.value, e.what());
    }
  }

  auto res = net::web_server::string_res_t{boost::beast::http::status::ok,
                                           req.version()};
  res.body() = history->since(since, body_limit);
  return res;
}

}  // namespace auser