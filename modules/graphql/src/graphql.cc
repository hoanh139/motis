#include "motis/graphql/graphql.h"

#include <chrono>
#include "graphqlservice/JSONResponse.h"
#include "otp/PlanObject.h"
#include "otp/QueryTypeObject.h"
#include "otp/otpSchema.h"

namespace mm = motis::module;
namespace fbb = flatbuffers;
namespace gql = graphql;
namespace otp = gql::otp;
namespace otpo = otp::object;
using namespace std::string_view_literals;

namespace motis::graphql {

using String = gql::response::StringType;
using Float = gql::response::FloatType;
using Boolean = gql::response::BooleanType;
using Long = gql::response::IntType;
using Int = gql::response::IntType;

struct debugOutput{
  gql::response::ScalarType gettotalTime() const noexcept {
    return gql::response::ScalarType{1};
  }
  gql::response::ScalarType getpathCalculationTime() const noexcept {
    return gql::response::ScalarType{1};
  }
  gql::response::ScalarType getprecalculationTime() const noexcept {
    return gql::response::ScalarType{1};
  }
  gql::response::ScalarType getrenderingTime() const noexcept {
    return gql::response::ScalarType{1};
  }
  bool gettimedOut() const noexcept {
    return true;
  }
};

struct Place {
  explicit Place(long lat, long lon, long arrival, long departure)
      :_lat(lat), _lon(lon), _arrival(arrival), _departure(departure){}
  /*
  gql::response::ScalarType getLat() const noexcept {
return gql::response::ScalarType{"a"};
}
gql::response::ScalarType getLon() const noexcept {
return gql::response::ScalarType{"b"};
}
gql::response::ScalarType getArrival() const noexcept {
return gql::response::ScalarType{"a"};
}
gql::response::ScalarType getDeparture() const noexcept {
return gql::response::ScalarType{"b"};
}
   */
  long getLat() const noexcept;
  long getLon() const noexcept;
  long getArrival() const noexcept;
  long getDeparture() const noexcept;

  const long _lat;
  const long _lon;
  const long _arrival;
  const long _departure;
};

struct Plan {
  explicit Plan(
      gql::response::StringType const& date,
      gql::response::StringType const& time, otp::InputCoordinates from,
      otp::InputCoordinates to, String fromPlace, String toPlace,
      Boolean wheelchair, int numItineraries, Long searchWindow,
      String pageCursor, Float bikeReluctance, Float bikeWalkingReluctance,
      Float carReluctance, Float walkReluctance, Float waitReluctance,
      Float walkSpeed, Float bikeSpeed, Int bikeSwitchTime, Int bikeSwitchCost,
      otp::OptimizeType optimize, otp::InputTriangle triangle, Boolean arriveBy,
      otp::InputPreferred preferred, otp::InputUnpreferred unpreferred,
      Int walkBoardCost, Int bikeBoardCost, otp::InputBanned banned,
      Int transferPenalty, std::vector<otp::TransportMode> transportModes,
      otp::InputModeWeight modeWeight, Boolean debugItineraryFilter,
      Boolean allowKeepingRentedBicycleAtDestination,
      Int keepingRentedBicycleAtDestinationCost, Int boardSlack,
      Int alightSlack, Int minTransferTime, Int nonpreferredTransferPenalty,
      String startTransitStopId, Boolean omitCanceled,
      Boolean ignoreRealtimeUpdates, String locale,
      std::vector<String> allowedTicketTypes,
      std::vector<String> allowedVehicleRentalNetworks,
      std::vector<String> bannedVehicleRentalNetworks, Float walkSafetyFactor,
      otp::VehicleParkingInput parking, Float maxWalkDistance,
      Float walkOnStreetReluctance, Float waitAtBeginningFactor, Boolean batch,
      Boolean allowBikeRental, Long claimInitialWait,
      Boolean reverseOptimizeOnTheFly, Boolean disableRemainingWeightHeuristic,
      Boolean compactLegsByReversedSearch,
      std::vector<String> allowedBikeRentalNetworks, Int maxPreTransitTime,
      Float carParkCarLegWeight, Int heuristicStepsPerMainStep,
      Float itineraryFiltering, String startTransitTripId)
      : _from(std::make_shared<Place>(from.lat,from.lon,1.0,1.0)),
        _to(std::make_shared<Place>(to.lat,to.lon,1.0,1.0),
            debugOutput(std::make_shared<otpo::debugOutput>())){}

  gql::response::Value getDate() const noexcept;
  std::shared_ptr<Place> getFrom() const noexcept{
    return _from;
  }

  std::shared_ptr<Place> _from;
  std::shared_ptr<Place> _to;
  std::vector<std::shared_ptr<otpo::Itinerary>> itineraries;
  std::vector<std::shared_ptr<gql::response::Value>> messageEnums;
  std::vector<std::shared_ptr<gql::response::Value>> messageStrings;
  std::vector<std::shared_ptr<otpo::RoutingError>> routingErrors;
  std::shared_ptr<otpo::debugOutput> debugOutput;
};

struct Query {
  std::shared_ptr<otpo::Plan> plan(
      gql::response::StringType const& date,
      gql::response::StringType const& time, otp::InputCoordinates from,
      otp::InputCoordinates to, String fromPlace, String toPlace,
      Boolean wheelchair, int numItineraries, Long searchWindow,
      String pageCursor, Float bikeReluctance, Float bikeWalkingReluctance,
      Float carReluctance, Float walkReluctance, Float waitReluctance,
      Float walkSpeed, Float bikeSpeed, Int bikeSwitchTime, Int bikeSwitchCost,
      otp::OptimizeType optimize, otp::InputTriangle triangle, Boolean arriveBy,
      otp::InputPreferred preferred, otp::InputUnpreferred unpreferred,
      Int walkBoardCost, Int bikeBoardCost, otp::InputBanned banned,
      Int transferPenalty, std::vector<otp::TransportMode> transportModes,
      otp::InputModeWeight modeWeight, Boolean debugItineraryFilter,
      Boolean allowKeepingRentedBicycleAtDestination,
      Int keepingRentedBicycleAtDestinationCost, Int boardSlack,
      Int alightSlack, Int minTransferTime, Int nonpreferredTransferPenalty,
      String startTransitStopId, Boolean omitCanceled,
      Boolean ignoreRealtimeUpdates, String locale,
      std::vector<String> allowedTicketTypes,
      std::vector<String> allowedVehicleRentalNetworks,
      std::vector<String> bannedVehicleRentalNetworks, Float walkSafetyFactor,
      otp::VehicleParkingInput parking, Float maxWalkDistance,
      Float walkOnStreetReluctance, Float waitAtBeginningFactor, Boolean batch,
      Boolean allowBikeRental, Long claimInitialWait,
      Boolean reverseOptimizeOnTheFly, Boolean disableRemainingWeightHeuristic,
      Boolean compactLegsByReversedSearch,
      std::vector<String> allowedBikeRentalNetworks, Int maxPreTransitTime,
      Float carParkCarLegWeight, Int heuristicStepsPerMainStep,
      Float itineraryFiltering, String startTransitTripId) const noexcept {

    std::shared_ptr<otpo::Plan> result =
        std::make_shared<otpo::Plan>(std::make_shared<Plan>());

    return result;
  }  // namespace motis::graphql
};

graphql::graphql() : module("GraphQL", "graphql") {}

void graphql::init(motis::module::registry& reg) {
  reg.register_op(
      "/graphql",
      [](mm::msg_ptr const& msg) {
        auto const req = motis_content(HTTPRequest, msg);

        std::shared_ptr<otpo::debugOutput> tryyy =
            std::make_shared<otpo::debugOutput>(std::make_shared<debugOutput>());

        std::shared_ptr<gql::service::Request> service =
            std::make_shared<otp::Operations>(std::make_shared<Query>());























        auto payload = gql::response::parseJSON(req->content()->str());
        auto variablesItr = payload.find("variables"sv);
        if (variablesItr == payload.end() ||
            variablesItr->second.type() != gql::response::Type::Map) {
          throw std::runtime_error{"variables missing"};
        }
        auto variables = gql::response::Value{gql::response::Type::Map};
        for (auto [k, v] : variablesItr->second) {
          variables.emplace_back(std::move(k), std::move(v));
        }

        auto queryItr = payload.find("query"sv);
        if (queryItr == payload.end() ||
            queryItr->second.type() != gql::response::Type::String) {
          throw std::runtime_error("query missing");
        }

        auto query = gql::peg::parseString(
            queryItr->second.get<gql::response::StringType>());
        auto const response = gql::response::toJSON(
            service
                ->resolve({.query = query,
                           .operationName = "GetTrip"sv,
                           .variables = std::move(variables)})
                .get());

        mm::message_creator mc;
        mc.create_and_finish(
            MsgContent_HTTPResponse,
            CreateHTTPResponse(
                mc, HTTPStatus_OK,
                mc.CreateVector(std::vector<fbb::Offset<HTTPHeader>>{}),
                mc.CreateString("response"))
                .Union());
        return make_msg(mc);
      },
      {});
}

}  // namespace motis::graphql