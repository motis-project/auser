#include "gtest/gtest.h"

#include "net/web_server/query_router.h"

#include "auser/endpoints/client_status.h"

constexpr auto const start =
    R"(<?xml version="1.0" encoding="iso-8859-1"?>
<ClientStatusAntwort>
  <Status Zst=")";

constexpr auto const end =
    R"(" Ergebnis="ok" />
  <AktiveAbos />
</ClientStatusAntwort>
)";

TEST(auser, client_status) {
  auto const cfg = auser::config{};
  auto const conn = auser::connection{cfg};
  auto const cs = auser::client_status{conn};
  auto const req = net::route_request{net::request{}, ""};
  auto const res0 =
      std::get<net::web_server::string_res_t>(cs(req, true)).body();
  EXPECT_TRUE(res0.starts_with(start));
  EXPECT_TRUE(res0.ends_with(end));
}