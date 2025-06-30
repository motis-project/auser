#include "auser/history.h"

#include "fmt/printf.h"

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

void history::add(std::string_view s, time_t::rep k) {
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

  while (contains(k)) {
    ++k;
  }
  index_.emplace_back(k, data_.size());

  auto const update = s.substr(start, end + kEnd.size() - start);

  data_ += update;
  data_ += '\n';
}

void history::add(std::string_view s) {
  add(s, now().time_since_epoch().count());
}

std::pair<std::string_view, time_t::rep> history::since(
    time_t::rep const k, std::uint32_t const max) const {
  auto start = begin(index_);
  while (start != end(index_) && start->first <= k) {
    ++start;
  }

  if (start != end(index_)) {
    auto const data_end = [&](auto const i) {
      return i + 1 == index_.size() ? data_.size() : index_[i + 1].second;
    };

    auto const from = start->second;
    for (auto r = index_.size(); r-- > 0;) {
      auto const until = data_end(r);
      if (until - from <= (max - kMaxIdLength)) {
        return {{begin(data_) + static_cast<long>(from),
                 begin(data_) + static_cast<long>(until)},
                index_[r].first};
      }
    }
  }

  return {"", k};
}

history copy_suffix(history const& h, time_t::rep const discard_before) {
  auto copy = history{};

  for (auto const& kv : h.index_) {
    if (kv.first >= discard_before) {
      copy.index_.emplace_back(kv);
    }
  }

  if (!copy.index_.empty()) {
    copy.data_ += std::string_view{
        begin(h.data_) + static_cast<long>(copy.index_.front().second),
        end(h.data_)};
  }

  auto const shift = h.data_.size() - copy.data_.size();
  for (auto& kv : copy.index_) {
    kv.second -= shift;
  }

  return copy;
}

}  // namespace auser