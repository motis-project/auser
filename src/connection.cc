#include "auser/connection.h"

#include "fmt/format.h"

#include "auser/xml.h"

namespace auser {

connection::connection(auser::config const& cfg)
    : cfg_{cfg},
      client_status_path_{
          fmt::format("/{}/aus/clientstatus.xml", cfg.server_name_)},
      data_ready_path_{
          fmt::format("/{}/aus/datenbereit.xml", cfg.server_name_)},
      server_status_addr_{fmt::format(
          "{}/{}/aus/status.xml", cfg.server_addr_, cfg.client_name_)},
      subscription_addr_{fmt::format(
          "{}/{}/aus/aboverwalten.xml", cfg.server_addr_, cfg.client_name_)},
      get_upstream_data_addr_{fmt::format(
          "{}/{}/aus/datenabrufen.xml", cfg.server_addr_, cfg.client_name_)} {}

connection::connection(connection&& other) noexcept
    : cfg_{other.cfg_},
      client_status_path_{std::move(other.client_status_path_)},
      data_ready_path_{std::move(other.data_ready_path_)},
      server_status_addr_{std::move(other.server_status_addr_)},
      subscription_addr_{std::move(other.subscription_addr_)},
      get_upstream_data_addr_{std::move(other.get_upstream_data_addr_)} {}

void connection::start() {
  prev_id_ = id_.exchange(now().time_since_epoch().count());
}

std::string connection::make_get_upstream_req() const {
  auto doc = make_xml_doc();
  auto fetch_data_node = doc.append_child("DatenAbrufenAnfrage");
  fetch_data_node.append_attribute("Sender") = cfg_.client_name_.c_str();
  fetch_data_node.append_attribute("Zst") = timestamp(now()).c_str();
  auto all_datasets_node = fetch_data_node.append_child("DatensatzAlle");
  all_datasets_node.append_child(pugi::node_pcdata).set_value("true");
  return xml_to_str(doc);
}

std::string connection::get_subscription_id() const {
  return std::to_string(
      std::chrono::round<std::chrono::seconds>(time_t{time_t::duration{id_}})
          .time_since_epoch()
          .count());
}

}  // namespace auser