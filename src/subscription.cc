#include "vdvauser/subscription.h"

#include "boost/asio/co_spawn.hpp"
#include "boost/asio/detached.hpp"
#include "boost/asio/redirect_error.hpp"
#include "boost/asio/steady_timer.hpp"
#include "boost/url.hpp"

#include "fmt/printf.h"

#include "vdvauser/config.h"
#include "vdvauser/connection.h"
#include "vdvauser/xml.h"
#include "vdvauser/http.h"

namespace vdvauser {

    pugi::xml_node add_sub_req_node(pugi::xml_node &node,
                                    std::string const &sender) {
        auto sub_req_node = node.append_child("AboAnfrage");
        sub_req_node.append_attribute("Sender") = sender.c_str();
        sub_req_node.append_attribute("Zst") = timestamp(now()).c_str();
        return sub_req_node;
    }

    std::string unsubscribe_body(connection const &con) {
        auto doc = make_xml_doc();
        add_sub_req_node(doc, con.cfg_.client_name_)
                .append_child("AboLoeschenAlle")
                .append_child(pugi::node_pcdata)
                .set_value("true");
        return xml_to_str(doc);
    }

    std::string subscribe_body(config const &c, connection &con) {
        auto doc = make_xml_doc();
        auto sub_req_node = add_sub_req_node(doc, con.cfg_.client_name_);
        auto sub_node = sub_req_node.append_child("AboAUS");
        sub_node.append_attribute("AboID") = std::to_string(con.id_).data();
        sub_node.append_attribute("VerfallZst") =
                timestamp(
                        now() +
                        std::chrono::seconds{c.subscription_duration_})
                        .c_str();
        auto hysteresis_node = sub_node.append_child("Hysterese");
        hysteresis_node.append_child(pugi::node_pcdata)
                .set_value(std::to_string(con.cfg_.hysteresis_).c_str());
        auto lookahead_node = sub_node.append_child("Vorschauzeit");
        lookahead_node.append_child(pugi::node_pcdata)
                .set_value(
                        std::to_string(std::chrono::round<std::chrono::minutes>(
                                std::chrono::seconds{
                                        c.lookahead_})
                                               .count())
                                .c_str());
        return xml_to_str(doc);
    }

    boost::asio::awaitable<void> unsubscribe(boost::asio::io_context &ioc,
                                             config const &c,
                                             connection &con) {
        co_await boost::asio::co_spawn(
                ioc,
                [&c, &con]() -> boost::asio::awaitable<void> {
                    try {
                        auto const res = co_await http_POST(
                                boost::urls::url{con.subscription_addr_}, kHeaders,
                                unsubscribe_body(con),
                                std::chrono::seconds{c.timeout_});
                        if (res.result_int() != 200U) {
                            fmt::println("[vdvaus] unsubscribe failed: {}",
                                         get_http_body(res));
                        }
                    } catch (std::exception const &e) {
                        fmt::println("[vdvaus] unsubscribe failed: {}", e.what());
                    }
                },
                boost::asio::use_awaitable);
    }

    boost::asio::awaitable<void> subscribe(boost::asio::io_context &ioc,
                                           config const &c,
                                           connection &con) {
        co_await boost::asio::co_spawn(
                ioc,
                [&c, &con]() -> boost::asio::awaitable<void> {
                    try {
                        con.start();
                        auto const res = co_await http_POST(
                                boost::urls::url{con.subscription_addr_}, kHeaders,
                                subscribe_body(c, con),
                                std::chrono::seconds{c.timeout_});
                        if (res.result_int() != 200U) {
                            con.stop();
                            fmt::println("[vdvaus] subscribe failed: {}",
                                         get_http_body(res));
                        }
                    } catch (std::exception const &e) {
                        con.stop();
                        fmt::println("[vdvaus] subscribe failed: {}", e.what());
                    }
                },
                boost::asio::use_awaitable);
    }

    void subscription(boost::asio::io_context &ioc, config const &c, connection &con) {
        boost::asio::co_spawn(
                ioc,
                [&c, &con, &ioc]() -> boost::asio::awaitable<void> {
                    auto executor = co_await boost::asio::this_coro::executor;
                    auto timer = boost::asio::steady_timer{executor};
                    auto ec = boost::system::error_code{};
                    while (true) {
                        auto const start = std::chrono::steady_clock::now();

                        co_await unsubscribe(ioc, c, con);
                        co_await subscribe(ioc, c, con);

                        timer.expires_at(
                                start +
                                std::chrono::seconds{c.subscription_duration_});
                        co_await timer.async_wait(
                                boost::asio::redirect_error(boost::asio::use_awaitable, ec));
                        if (ec == boost::asio::error::operation_aborted) {
                            co_return;
                        }
                    }
                },
                boost::asio::detached);
    }

    void shutdown(boost::asio::io_context &ioc, config const &c, connection &con) {
        boost::asio::co_spawn(
                ioc,
                [&c, &con, &ioc]() -> boost::asio::awaitable<void> {
                    co_await unsubscribe(ioc, c, con);
                },
                boost::asio::detached);
    }

}  // namespace vdvauser