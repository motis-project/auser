#include "auser/endpoints/fetch.h"

#include "fmt/printf.h"

#include "auser/xml.h"

namespace auser {

std::string fetch::operator()(boost::urls::url_view const& url) const {
  auto const history = history_;

  auto since = time_t::rep{0};
  auto const params = url.params();
  if (params.contains("since")) {
    auto const since_param_str = (*url.params().find_last("since")).value;
    try {
      since = std::stol(since_param_str);
    } catch (std::exception const& e) {
      fmt::println("[fetch] could not parse parameter since: {}, {}",
                   since_param_str, e.what());
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

  auto ret = std::stringstream{};
  doc.save(ret);
  return ret.str();
}

}  // namespace auser