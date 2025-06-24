#include "gtest/gtest.h"

#include "net/web_server/query_router.h"

#include "auser/endpoints/data_ready.h"

constexpr auto const start =
    R"(<?xml version="1.0" encoding="iso-8859-1"?>
<DatenBereitAntwort>
  <Bestaetigung Zst=")";

constexpr auto const end =
    R"(" Ergebnis="ok" Fehlernummer="0" />
</DatenBereitAntwort>
)";

TEST(auser, data_ready) {
  auto const dr = auser::data_ready{};
  auto const req = net::route_request{net::request{}, ""};
  auto const res0 =
      std::get<net::web_server::string_res_t>(dr(req, true)).body();
  EXPECT_TRUE(res0.starts_with(start));
  EXPECT_TRUE(res0.ends_with(end));
}