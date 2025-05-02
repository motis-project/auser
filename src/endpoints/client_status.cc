#include "auser/endpoints/client_status.h"

#include "fmt/printf.h"

#include "auser/connection.h"
#include "auser/xml.h"

namespace auser {

std::string client_status::operator()(std::string_view s) const {
  fmt::println("received client_status_request: {}", s);

  auto doc = make_xml_doc();
  auto client_status_res_node = doc.append_child("ClientStatusAntwort");

  auto status_node = client_status_res_node.append_child("Status");
  status_node.append_attribute("Zst") = timestamp(now()).c_str();
  status_node.append_attribute("Ergebnis") = "ok";

  auto start_time_node = client_status_res_node.append_child("StartDienstZst");
  start_time_node.append_child(pugi::node_pcdata)
      .set_value(timestamp(conn_.start_).c_str());

  auto active_subs_node = client_status_res_node.append_child("AktiveAbos");
  if (conn_.id_ != 0) {
    auto active_sub_node = active_subs_node.append_child("AboAUS");
    active_sub_node.append_attribute("AboID") =
        std::to_string(conn_.id_).c_str();
    active_sub_node.append_attribute("VerfallZst") =
        timestamp(conn_.start_.load() +
                  std::chrono::seconds{conn_.cfg_.subscription_duration_})
            .c_str();
  }

  return xml_to_str(doc);
}

}  // namespace auser