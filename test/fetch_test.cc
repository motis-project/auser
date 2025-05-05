#include "gtest/gtest.h"

#include "auser/fetch.h"
#include "auser/xml.h"

constexpr auto const kWeitereDatenTrue = R"__(
<DatenAbrufenAntwort>
  <WeitereDaten>true</WeitereDaten>
</DatenAbrufenAntwort>
)__";

constexpr auto const kWeitereDatenFalse = R"__(
<DatenAbrufenAntwort>
  <WeitereDaten>false</WeitereDaten>
</DatenAbrufenAntwort>
)__";

constexpr auto const kWeitereDatenMissing = R"__(
<DatenAbrufenAntwort>
</DatenAbrufenAntwort>
)__";

TEST(auser, but_wait_there_is_more) {
  EXPECT_TRUE(auser::but_wait_there_is_more(auser::parse(kWeitereDatenTrue)));
  EXPECT_FALSE(auser::but_wait_there_is_more(auser::parse(kWeitereDatenFalse)));
  EXPECT_FALSE(
      auser::but_wait_there_is_more(auser::parse(kWeitereDatenMissing)));
}