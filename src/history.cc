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

double byte_to_mb(auto const x) {
  return static_cast<double>(x) / (1024.0 * 1024.0);
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

  fmt::println("[history][add] {} + {:.2f} MB --> {} ({:.2f} MB)",
               index_.size() > 1 ? std::next(rbegin(index_))->first : 0,
               byte_to_mb(update.size()), index_.back().first,
               byte_to_mb(data_.size()));
}

void history::add(std::string_view s) {
  add(s, now().time_since_epoch().count());
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
      auto const vdvaus_emitted =
          std::string_view{begin(data_) + static_cast<long>(from),
                           begin(data_) + static_cast<long>(until)};
      fmt::println("[history][since] {} --> {} ({:.2f} MB)", k, index_[r].first,
                   byte_to_mb(vdvaus_emitted.size()));
      return std::format("{}{}", vdvaus_emitted, index_[r].first);
    }
  }

  return std::to_string(k);
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

  fmt::println("[history][copy_suffix] discarding before {} (- {:.2f} MB)",
               discard_before,
               byte_to_mb(h.data_.size()) - byte_to_mb(copy.data_.size()));
  return copy;
}

}  // namespace auser