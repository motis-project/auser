#include "auser/history.h"

using namespace std::string_view_literals;

namespace auser {

constexpr auto const kMaxIdLength = std::numeric_limits<time_t::rep>::digits10;

bool history::empty() const { return index_.empty(); }

bool history::contains(time_t::rep const k) const {
  for (auto r = rbegin(index_); r != rend(index_); ++r) {
    if (r->first < k) {
      return false;
    } else if (r->first == k) {
      return true;
    }
  }
  return false;
}

void history::add(std::string_view s) {
  static constexpr auto const kStart = "<IstFahrt"sv;
  static constexpr auto const kEnd = "IstFahrt>"sv;

  auto const start = s.find(kStart);
  if (start == std::string_view::npos) {
    return;
  }
  auto const end = s.rfind(kEnd);
  if (end == std::string_view::npos) {
    return;
  }

  auto k = now().time_since_epoch().count();
  while (contains(k)) {
    ++k;
  }
  index_.emplace_back(k, data_.size());

  data_ += s.substr(start, end + kEnd.size());
  data_ += '\n';
}

std::string history::since(time_t::rep const k, std::uint32_t const max) const {
  auto start = begin(index_);
  while (start != end(index_) && start->first <= k) {
    ++start;
  }

  if (start == end(index_)) {
    return std::to_string(k);
  }

  auto const data_end = [&](auto const i) {
    return i + 1 == index_.size() ? data_.size() : index_[i + 1].second;
  };

  auto const from = start->second;
  for (auto r = index_.size(); r-- > 0;) {
    auto const until = data_end(r);
    if (until - from <= (max - kMaxIdLength)) {
      return std::format(
          "{}{}",
          std::string_view{begin(data_) + static_cast<long>(from),
                           begin(data_) + static_cast<long>(until)},
          index_[r].first);
    }
  }

  return std::to_string(k);
}

}  // namespace auser