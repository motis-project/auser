#include "vdvauser/endpoints/data_ready.h"

#include "fmt/printf.h"

#include "vdvauser/xml.h"

namespace vdvauser {

    std::string data_ready::operator()(std::string_view s) const {
        fmt::println("received data_ready_request: {}", s);

        auto doc = make_xml_doc();
        auto data_ready_node = doc.append_child("DatenBereitAntwort");

        auto ack_node = data_ready_node.append_child("Bestaetigung");
        ack_node.append_attribute("Zst") = timestamp(now()).c_str();
        ack_node.append_attribute("Ergebnis") = "ok";
        ack_node.append_attribute("Fehlernummer") = "0";

        return xml_to_str(doc);
    }

}  // namespace vdvauser