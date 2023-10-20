#pragma once

#include "motis/module/module.h"

namespace motis::geocoding {

struct geocoding : public motis::module::module {
  geocoding();
  void init(motis::module::registry&) override;
};

}  // namespace motis::geocoding
