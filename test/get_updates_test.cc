#include "gtest/gtest.h"

#include "auser/endpoints/get_updates.h"
#include "auser/xml.h"

constexpr auto const update0 = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<DatenAbrufenAntwort>
  <Bestaetigung Zst="2024-07-10T00:00:00" Ergebnis="ok" Fehlernummer="0" />
  <AUSNachricht AboID="1">
    <IstFahrt Zst="2024-07-10T00:00:00">
      <LinienID>AE</LinienID>
      <RichtungsID>1</RichtungsID>
      <FahrtRef>
        <FahrtID>
          <FahrtBezeichner>AE</FahrtBezeichner>
          <Betriebstag>2024-07-10</Betriebstag>
        </FahrtID>
      </FahrtRef>
      <Komplettfahrt>false</Komplettfahrt>
      <BetreiberID>MTA</BetreiberID>
      <IstHalt>
        <HaltID>A</HaltID>
        <Abfahrtszeit>2024-07-09T22:00:00</Abfahrtszeit>
        <Einsteigeverbot>false</Einsteigeverbot>
        <Aussteigeverbot>false</Aussteigeverbot>
        <Durchfahrt>false</Durchfahrt>
        <Zusatzhalt>false</Zusatzhalt>
      </IstHalt>
      <IstHalt>
        <HaltID>B</HaltID>
        <Ankunftszeit>2024-07-09T23:00:00</Ankunftszeit>
        <Abfahrtszeit>2024-07-09T23:00:00</Abfahrtszeit>
        <IstAnkunftPrognose>2024-07-09T23:30:00</IstAnkunftPrognose>
        <IstAbfahrtPrognose>2024-07-09T23:30:00</IstAbfahrtPrognose>
        <Einsteigeverbot>false</Einsteigeverbot>
        <Aussteigeverbot>false</Aussteigeverbot>
        <Durchfahrt>false</Durchfahrt>
        <Zusatzhalt>false</Zusatzhalt>
      </IstHalt>
      <IstHalt>
        <HaltID>D</HaltID>
        <Ankunftszeit>2024-07-10T01:00:00</Ankunftszeit>
        <Abfahrtszeit>2024-07-10T01:00:00</Abfahrtszeit>
        <IstAnkunftPrognose>2024-07-10T01:15:00</IstAnkunftPrognose>
        <IstAbfahrtPrognose>2024-07-10T01:15:00</IstAbfahrtPrognose>
        <Einsteigeverbot>false</Einsteigeverbot>
        <Aussteigeverbot>false</Aussteigeverbot>
        <Durchfahrt>false</Durchfahrt>
        <Zusatzhalt>false</Zusatzhalt>
      </IstHalt>
      <IstHalt>
        <HaltID>E</HaltID>
        <Ankunftszeit>2024-07-10T02:00:00</Ankunftszeit>
        <IstAnkunftPrognose>2024-07-10T02:00:00</IstAnkunftPrognose>
        <Einsteigeverbot>false</Einsteigeverbot>
        <Aussteigeverbot>false</Aussteigeverbot>
        <Durchfahrt>false</Durchfahrt>
        <Zusatzhalt>false</Zusatzhalt>
      </IstHalt>
      <LinienText>AE</LinienText>
      <ProduktID>Space Train</ProduktID>
      <RichtungsText>E</RichtungsText>
      <Zusatzfahrt>false</Zusatzfahrt>
      <FaelltAus>false</FaelltAus>
    </IstFahrt>
  </AUSNachricht>
</DatenAbrufenAntwort>
)";

constexpr auto const update1 = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<DatenAbrufenAntwort>
  <Bestaetigung Zst="2024-07-10T00:00:00" Ergebnis="ok" Fehlernummer="0" />
  <AUSNachricht AboID="1">
    <IstFahrt Zst="2024-07-10T00:00:00">
      <LinienID>AE</LinienID>
      <RichtungsID>1</RichtungsID>
      <FahrtRef>
        <FahrtID>
          <FahrtBezeichner>AE</FahrtBezeichner>
          <Betriebstag>2024-07-10</Betriebstag>
        </FahrtID>
      </FahrtRef>
      <Komplettfahrt>false</Komplettfahrt>
      <BetreiberID>MTA</BetreiberID>
      <IstHalt>
        <HaltID>A</HaltID>
        <Abfahrtszeit>2024-07-09T22:00:00</Abfahrtszeit>
        <IstAbfahrtPrognose>2024-07-09T23:00:00</IstAbfahrtPrognose>
        <Einsteigeverbot>false</Einsteigeverbot>
        <Aussteigeverbot>false</Aussteigeverbot>
        <Durchfahrt>false</Durchfahrt>
        <Zusatzhalt>false</Zusatzhalt>
      </IstHalt>
      <LinienText>AE</LinienText>
      <ProduktID>Space Train</ProduktID>
      <RichtungsText>E</RichtungsText>
      <Zusatzfahrt>false</Zusatzfahrt>
      <FaelltAus>false</FaelltAus>
    </IstFahrt>
  </AUSNachricht>
</DatenAbrufenAntwort>
)";

constexpr auto const update2 = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<DatenAbrufenAntwort>
  <Bestaetigung Zst="2024-07-10T00:00:00" Ergebnis="ok" Fehlernummer="0" />
  <AUSNachricht AboID="1">
    <IstFahrt Zst="2024-07-10T00:00:00">
      <LinienID>AE</LinienID>
      <RichtungsID>1</RichtungsID>
      <FahrtRef>
        <FahrtID>
          <FahrtBezeichner>AE</FahrtBezeichner>
          <Betriebstag>2024-07-10</Betriebstag>
        </FahrtID>
      </FahrtRef>
      <BetreiberID>MTA</BetreiberID>
      <IstHalt>
        <HaltID>A</HaltID>
        <Abfahrtszeit>2024-07-09T22:00:00</Abfahrtszeit>
        <IstAbfahrtPrognose>2024-07-09T22:01:00</IstAbfahrtPrognose>
        <Einsteigeverbot>false</Einsteigeverbot>
        <Aussteigeverbot>false</Aussteigeverbot>
        <Durchfahrt>false</Durchfahrt>
        <Zusatzhalt>false</Zusatzhalt>
      </IstHalt>
      <IstHalt>
        <HaltID>B</HaltID>
        <Ankunftszeit>2024-07-09T23:00:00</Ankunftszeit>
        <Abfahrtszeit>2024-07-09T23:00:00</Abfahrtszeit>
        <IstAnkunftPrognose>2024-07-09T22:55:00</IstAnkunftPrognose>
        <IstAbfahrtPrognose>2024-07-09T23:05:00</IstAbfahrtPrognose>
        <Einsteigeverbot>false</Einsteigeverbot>
        <Aussteigeverbot>false</Aussteigeverbot>
        <Durchfahrt>false</Durchfahrt>
        <Zusatzhalt>false</Zusatzhalt>
      </IstHalt>
      <IstHalt>
        <HaltID>C</HaltID>
        <Ankunftszeit>2024-07-10T00:00:00</Ankunftszeit>
        <Abfahrtszeit>2024-07-10T00:00:00</Abfahrtszeit>
        <IstAnkunftPrognose>2024-07-09T23:55:00</IstAnkunftPrognose>
        <IstAbfahrtPrognose>2024-07-10T00:05:00</IstAbfahrtPrognose>
        <Einsteigeverbot>false</Einsteigeverbot>
        <Aussteigeverbot>false</Aussteigeverbot>
        <Durchfahrt>false</Durchfahrt>
        <Zusatzhalt>false</Zusatzhalt>
      </IstHalt>
      <IstHalt>
        <HaltID>E</HaltID>
        <Ankunftszeit>2024-07-10T02:00:00</Ankunftszeit>
        <IstAnkunftPrognose>2024-07-10T02:07:00</IstAnkunftPrognose>
        <Einsteigeverbot>false</Einsteigeverbot>
        <Aussteigeverbot>false</Aussteigeverbot>
        <Durchfahrt>false</Durchfahrt>
        <Zusatzhalt>false</Zusatzhalt>
      </IstHalt>
      <LinienText>AE</LinienText>
      <ProduktID>Space Train</ProduktID>
      <RichtungsText>E</RichtungsText>
      <Zusatzfahrt>false</Zusatzfahrt>
      <FaelltAus>false</FaelltAus>
    </IstFahrt>
  </AUSNachricht>
</DatenAbrufenAntwort>
)";

TEST(auser, get_updates) {
  auto updates = std::make_shared<auser::updates_t>();
  updates->emplace(1, auser::parse(update0));
  updates->emplace(2, auser::parse(update1));
  updates->emplace(3, auser::parse(update2));

  auto const gu = auser::get_updates{updates};
  auto const res_default = gu("http://www.example.com/api/v1/get_updates");
  auto const res_0 = gu("http://www.example.com/api/v1/get_updates?since=0");
  EXPECT_EQ(res_default, res_0);

  std::cout << res_0 << "\n";
}