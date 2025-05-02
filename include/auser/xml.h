#pragma once

#include "pugixml.hpp"

#include "auser/time.h"

namespace auser {

    pugi::xml_document make_xml_doc();

    std::string xml_to_str(pugi::xml_document const &doc);

    pugi::xml_document parse(std::string const &);

}  // namespace auser