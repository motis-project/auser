#include "auser/endpoints/get_updates.h"

#include "fmt/printf.h"

namespace auser {

std::string get_updates::operator()(boost::urls::url_view const& url) const {

  auto since = time_t::rep{0};
  auto const params = url.params();
  if (params.contains("since")) {
    auto const since_param_str = (*url.params().find_last("since")).value;
    try {
      since = std::stol(since_param_str);
    } catch (std::exception const& e) {
      fmt::println("[get_updates] could not parse parameter since: {}, {}",
                   since_param_str, e.what());
    }
  }

  auto const& [t, doc] = *updates_->upper_bound(since);
}

}  // namespace auser