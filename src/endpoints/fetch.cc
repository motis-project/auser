#include "auser/endpoints/fetch.h"

#include "fmt/printf.h"

#include "auser/xml.h"

namespace auser {

constexpr auto const kMaxRides = 1000U;

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
  auto auser_id =
      std::max(time_t::rep{0},
               std::min(since, history->empty() ? time_t::rep{0}
                                                : rbegin(*history)->first));
  auto n_rides = 0U;
  for (auto kv = history->upper_bound(since); kv != end(*history); ++kv) {
    auto const& [k, v] = *kv;
    auto const nodes = v.select_nodes("//IstFahrt");
    if (n_rides + nodes.size() > kMaxRides) {
      break;
    }
    for (auto const n : nodes) {
      msg.append_copy(n.node());
    }
    n_rides += nodes.size();
    auser_id = k;
  }
  msg.append_attribute("auser_id") = auser_id;
  fmt::println("[fetch] {} --> {} ({} rides)", since, auser_id, n_rides);

  auto res = net::web_server::string_res_t{boost::beast::http::status::ok,
                                           req.version()};
  res.body() = xml_to_str(doc);
  return res;
}

}  // namespace auser