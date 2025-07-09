#include "auser/endpoints/fetch.h"

#include "fmt/printf.h"

namespace auser {

using namespace std::literals;

constexpr auto const kDefaultBodyLimit = 8U * 1024U * 1024U;
constexpr auto const kMaxBodyLimit = 128U * 1024U * 1024U;

constexpr auto const wrapper_front =
    R"(<?xml version="1.0" encoding="iso-8859-1"?>
<AUSNachricht auser_id=")"sv;

constexpr auto const wrapper_mid = R"(">
)"sv;

constexpr auto const wrapper_back = R"(</AUSNachricht>
)"sv;

net::reply fetch::operator()(net::route_request const& req, bool) const {
  auto const h = h_;

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
  since = std::min(std::max(since, time_t::rep{0}),
                   h->index_.empty() ? time_t::rep{0} : h->index_.back().first);
  body_limit = std::min(body_limit, kMaxBodyLimit);

  auto const [rides, auser_id] =
      h->since(since, (body_limit - (wrapper_front.size() + wrapper_mid.size() +
                                     wrapper_back.size())));

  auto res = net::web_server::string_res_t{boost::beast::http::status::ok,
                                           req.version()};
  res.body() = fmt::format("{}{}{}{}{}", wrapper_front, auser_id, wrapper_mid,
                           rides, wrapper_back);

  fmt::println("[fetch] {} --> {} ({:.2f} MB)", since, auser_id,
               static_cast<double>(res.body().size()) / (1024.0 * 1024.0));
  std::cout.flush();
  return res;
}

}  // namespace auser