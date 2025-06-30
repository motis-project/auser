#include "gtest/gtest.h"

#include "auser/history.h"

using namespace auser;

TEST(history, add) {
  auto h = history{};
  h.add("SPAM<IstFahrt>Hello</IstFahrt>SPAM", 1);
  EXPECT_EQ("<IstFahrt>Hello</IstFahrt>\n", h.data_);
  EXPECT_EQ(1, h.index_[0].first);
  EXPECT_EQ(0, h.index_[0].second);
  h.add("<IstFahrt>World</IstFahrt>", 2);
  EXPECT_EQ("<IstFahrt>Hello</IstFahrt>\n<IstFahrt>World</IstFahrt>\n",
            h.data_);
  EXPECT_EQ(1, h.index_[0].first);
  EXPECT_EQ(0, h.index_[0].second);
  EXPECT_EQ(2, h.index_[1].first);
  EXPECT_EQ(27, h.index_[1].second);
  h = copy_suffix(h, 2);
  EXPECT_EQ(2, h.index_[0].first);
  EXPECT_EQ(0, h.index_[0].second);
  EXPECT_EQ("<IstFahrt>World</IstFahrt>\n", h.data_);
}