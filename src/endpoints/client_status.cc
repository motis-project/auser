#include "auser/endpoints/client_status.h"

#include "fmt/printf.h"

#include "auser/connection.h"
#include "auser/xml.h"

namespace auser {

net::reply client_status::operator()(net::route_request const& req,
                                     bool) const {
  fmt::println("[client_status] {}: {}", req.url_.data(), req.body().data());

  auto doc = make_xml_doc();
  auto client_status_res_node = doc.append_child("ClientStatusAntwort");

  auto status_node = client_status_res_node.append_child("Status");
  status_node.append_attribute("Zst") = timestamp(now()).c_str();
  status_node.append_attribute("Ergebnis") = "ok";

  auto const id = conn_.id_.load();

  if (id > 0) {
    auto start_time_node =
        client_status_res_node.append_child("StartDienstZst");
    start_time_node.append_child(pugi::node_pcdata)
        .set_value(timestamp(time_t{time_t::duration{id}}).c_str());
  }

  auto active_subs_node = client_status_res_node.append_child("AktiveAbos");
  if (id > 0) {
    auto active_sub_node = active_subs_node.append_child("AboAUS");
    active_sub_node.append_attribute("AboID") =
        conn_.get_subscription_id().data();
    active_sub_node.append_attribute("VerfallZst") =
        timestamp(time_t{time_t::duration{id}} +
                  std::chrono::seconds{conn_.cfg_.subscription_duration_})
            .c_str();
  }

  auto res = net::web_server::string_res_t{boost::beast::http::status::ok,
                                           req.version()};
  res.body() = xml_to_str(doc);
  return res;
}

}  // namespace auser