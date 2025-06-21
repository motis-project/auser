#include "auser/endpoints/fetch.h"

#include "fmt/printf.h"

#include "auser/xml.h"

namespace auser {

net::reply fetch::operator()(net::route_request const& req, bool) const {
  auto const history = history_;

  auto since = time_t::rep{0};
  for (auto const& p : req.url_.params()) {
    if (p.key == "since" && p.has_value) {
      try {
        since = std::stol(p.value);
      } catch (std::exception const& e) {
        fmt::println("[fetch] could not parse parameter since: {}, {}", p.value,
                     e.what());
      }
      break;
    }
  }

  auto doc = make_xml_doc();
  auto msg = doc.append_child("AUSNachricht");
  auto const now = history->empty() ? time_t::rep{0} : history->rbegin()->first;
  msg.append_attribute("auser_id") = now;
  auto n_rides = 0U;
  for (auto u = history->upper_bound(since); u != end(*history); ++u) {
    auto const& [t, d] = *u;
    for (auto const n : d.select_nodes("//AUSNachricht/*")) {
      msg.append_copy(n.node());
      ++n_rides;
    }
  }
  fmt::println("[fetch] {} --> {} (updates for {} rides)", since, now, n_rides);

  auto res = net::web_server::string_res_t{boost::beast::http::status::ok,
                                           req.version()};
  auto ss = std::stringstream{};
  doc.save(ss);
  res.body() = ss.str();
  return res;
}

}  // namespace auser