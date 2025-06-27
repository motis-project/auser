#include <boost/url/url.hpp>

#include "gtest/gtest.h"

#include "net/web_server/query_router.h"

#include "auser/endpoints/fetch.h"
#include "auser/xml.h"

constexpr auto const update_0 = R"(
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

constexpr auto const update_1 = R"(
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

constexpr auto const update_2 = R"(
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

constexpr auto const exp_empty =
    R"(<?xml version="1.0" encoding="iso-8859-1"?>
<AUSNachricht auser_id="0" />
)";

constexpr auto const exp_0 =
    R"(<?xml version="1.0" encoding="iso-8859-1"?>
<AUSNachricht auser_id="1">
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
)";

constexpr auto const exp_1 =
    R"(<?xml version="1.0" encoding="iso-8859-1"?>
<AUSNachricht auser_id="2">
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
)";

constexpr auto const exp_2 =
    R"(<?xml version="1.0" encoding="iso-8859-1"?>
<AUSNachricht auser_id="3">
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
)";

constexpr auto const exp_all =
    R"(<?xml version="1.0" encoding="iso-8859-1"?>
<AUSNachricht auser_id="3">
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
)";

constexpr auto const exp_greater_or_equal =
    R"(<?xml version="1.0" encoding="iso-8859-1"?>
<AUSNachricht auser_id="3" />
)";

void get_upstream_mock(std::shared_ptr<auser::history>& h,
                       std::string const& update) {
  auto copy = auser::history{};
  for (auto const& [k, v] : h->) {
    copy[k] = auser::make_xml_doc();
    for (auto const& c : v) {
      copy[k].append_copy(c);
    }
  }

  copy.try_emplace(static_cast<long>(copy.size()) + 1, auser::parse(update));

  h = std::make_shared<auser::history_t>(std::move(copy));
}

TEST(auser, fetch) {
  auto history = std::make_shared<auser::history_t>();
  auto const af = auser::fetch{history};

  auto const make_req_unpack_res = [&](std::string_view url) -> std::string {
    auto const req = net::route_request{net::request{}, url};
    return std::get<net::web_server::string_res_t>(af(req, true)).body();
  };

  EXPECT_EQ(exp_empty,
            make_req_unpack_res("http://www.example.com/auser/fetch"));
  EXPECT_EQ(exp_empty,
            make_req_unpack_res("http://www.example.com/auser/fetch?since=0"));
  EXPECT_EQ(exp_empty,
            make_req_unpack_res("http://www.example.com/auser/fetch?since=-1"));

  get_upstream_mock(history, update_0);

  auto const res_0 =
      make_req_unpack_res("http://www.example.com/auser/fetch?since=0");
  EXPECT_EQ(exp_0, res_0);
  EXPECT_EQ(res_0, make_req_unpack_res("http://www.example.com/auser/fetch"));

  get_upstream_mock(history, update_1);

  EXPECT_EQ(exp_1,
            make_req_unpack_res("http://www.example.com/auser/fetch?since=1"));

  get_upstream_mock(history, update_2);

  EXPECT_EQ(exp_2,
            make_req_unpack_res("http://www.example.com/auser/fetch?since=2"));
  EXPECT_EQ(exp_all,
            make_req_unpack_res("http://www.example.com/auser/fetch?since=0"));
  EXPECT_EQ(exp_all,
            make_req_unpack_res("http://www.example.com/auser/fetch?since=-1"));
  EXPECT_EQ(exp_greater_or_equal,
            make_req_unpack_res("http://www.example.com/auser/fetch?since=3"));
  EXPECT_EQ(exp_greater_or_equal,
            make_req_unpack_res("http://www.example.com/auser/fetch?since=42"));
}