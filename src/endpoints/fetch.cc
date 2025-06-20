#include "auser/endpoints/fetch.h"

#include "fmt/printf.h"

#include "auser/xml.h"

namespace auser {

std::string fetch::operator()(boost::urls::url_view const& url) const {

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

  auto id = since;
  auto doc = make_xml_doc();
  auto msg = doc.append_child("AUSNachricht");

  for (auto u = history_->upper_bound(since); u != end(*history_); ++u) {
    auto const& [t, d] = *u;

    for (auto const n : d.select_nodes("//AUSNachricht/*")) {
      msg.append_copy(n.node());
    }

    id = t;
  }

  msg.append_attribute("auser_id") = id;

  auto ret = std::stringstream{};
  doc.save(ret);
  return ret.str();
}

}  // namespace auser