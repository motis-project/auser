#include "auser/endpoints/data_ready.h"

#include "fmt/printf.h"

#include "auser/xml.h"

namespace auser {

net::reply data_ready::operator()(net::route_request const& req, bool) const {
  fmt::println("[data_ready] {}: {}", req.url_.data(), req.body().data());

  auto doc = make_xml_doc();
  auto data_ready_node = doc.append_child("DatenBereitAntwort");
  auto ack_node = data_ready_node.append_child("Bestaetigung");
  ack_node.append_attribute("Zst") = timestamp(now()).c_str();
  ack_node.append_attribute("Ergebnis") = "ok";
  ack_node.append_attribute("Fehlernummer") = "0";

  auto res = net::web_server::string_res_t{boost::beast::http::status::ok,
                                           req.version()};
  res.body() = xml_to_str(doc);
  return res;
}

}  // namespace auser