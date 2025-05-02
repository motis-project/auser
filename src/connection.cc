#include "vdvauser/connection.h"

#include "fmt/format.h"

#include "vdvauser/xml.h"

namespace vdvauser {

    connection::connection(vdvauser::config const &cfg)
            : cfg_{cfg},
              client_status_path_{
                      fmt::format("/{}/aus/clientstatus.xml", cfg.server_name_)},
              data_ready_path_{
                      fmt::format("/{}/aus/datenbereit.xml", cfg.server_name_)},
              server_status_addr_{
                      fmt::format("{}/{}/aus/status.xml", cfg.server_addr_, cfg.client_name_)},
              subscription_addr_{fmt::format(
                      "{}/{}/aus/aboverwalten.xml", cfg.server_addr_, cfg.client_name_)},
              fetch_data_addr_{fmt::format(
                      "{}/{}/aus/datenabrufen.xml", cfg.server_addr_, cfg.client_name_)} {}

    void connection::start() {
        id_ = std::chrono::round<std::chrono::seconds>(std::chrono::system_clock::now()).time_since_epoch().count();
        start_ = now();
    }

    void connection::stop() {
        id_ = 0;
        start_ = time::min();
    }

    std::string connection::make_fetch_req() {
        auto doc = make_xml_doc();
        auto fetch_data_node = doc.append_child("DatenAbrufenAnfrage");
        fetch_data_node.append_attribute("Sender") = cfg_.client_name_.c_str();
        fetch_data_node.append_attribute("Zst") = timestamp(now()).c_str();
        auto all_datasets_node = fetch_data_node.append_child("DatensatzAlle");
        all_datasets_node.append_child(pugi::node_pcdata).set_value("true");
        return xml_to_str(doc);
    }

} // namespace vdvauser