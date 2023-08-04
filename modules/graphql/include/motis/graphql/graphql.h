#pragma once

#include "motis/module/module.h"

namespace motis::graphql {

struct graphql : public motis::module::module {
  graphql();
  void init(motis::module::registry&) override;
};

}  // namespace motis::graphql
