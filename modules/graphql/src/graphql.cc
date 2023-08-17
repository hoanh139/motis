#include "motis/graphql/graphql.h"

#include "graphqlservice/JSONResponse.h"

#include "otp/NodeObject.h"
#include "otp/PlaceObject.h"
#include "otp/PlanObject.h"
#include "otp/QueryTypeObject.h"
#include "otp/debugOutputObject.h"
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

struct node {
  gql::service::TypeNames getTypeNames() const noexcept;
  gql::service::ResolverMap getResolvers() const noexcept;
};

struct debug_output {
  gql::response::Value getTotalTime() const noexcept {
    return gql::response::Value{1};
  }
  gql::response::Value getPathCalculationTime() const noexcept {
    return gql::response::Value{1};
  }
  gql::response::Value getPrecalculationTime() const noexcept {
    return gql::response::Value{1};
  }
  gql::response::Value getRenderingTime() const noexcept {
    return gql::response::Value{1};
  }
  bool getTimedOut() const noexcept { return true; }
};

struct place {
  gql::response::StringType getName() const noexcept { return ""; }
  otp::VertexType getVertextype() const noexcept {
    return otp::VertexType::NORMAL;
  }
  double getLat() const noexcept { return 0.0; }
  double getLon() const noexcept { return 0.0; }
  gql::response::Value getArrival() const noexcept {
    return gql::response::Value{"a"};
  }
  gql::response::Value getDeparture() const noexcept {
    return gql::response::Value{"b"};
  }
};

struct plan {
  gql::response::Value getDate() const noexcept {
    return gql::response::Value{"a"};
  }
  std::shared_ptr<otpo::Place> getFrom() const noexcept {
    return std::make_shared<otpo::Place>(std::make_shared<place>());
  }
  std::shared_ptr<otpo::Place> getTo() const noexcept {
    return std::make_shared<otpo::Place>(std::make_shared<place>());
  }
  //  std::vector<std::shared_ptr<otpo::Itinerary>> getItineraries() {
  //    return std::vector<std::shared_ptr<otpo::Itinerary>>();
  //  }
  //  std::vector<std::shared_ptr<gql::response::StringType>> getMessageEnums()
  //  {
  //    return std::vector<std::shared_ptr<gql::response::StringType>>();
  //  }
  //  std::vector<std::shared_ptr<gql::response::StringType>>
  //  getMessageStrings() {
  //    return std::vector<std::shared_ptr<gql::response::StringType>>();
  //  }
  //  std::vector<std::shared_ptr<gql::response::StringType>> getRoutingErrors()
  //  {
  //    return std::vector<std::shared_ptr<gql::response::StringType>>();
  //  }
  //  std::shared_ptr<otpo::debugOutput> getDebugOutput() {
  //    return std::make_shared<otpo::debugOutput>(
  //        std::make_shared<debug_output>());
  //  }
};

struct Query {
  std::shared_ptr<otpo::Plan> getPlan(
      std::optional<std::string>&& /*  dateArg */,
      std::optional<std::string>&& /*  timeArg */,
      std::unique_ptr<otp::InputCoordinates>&& /*  fromArg */,
      std::unique_ptr<otp::InputCoordinates>&& /*  toArg */,
      std::optional<std::string>&& /*  fromPlaceArg */,
      std::optional<std::string>&& /*  toPlaceArg */,
      std::optional<bool>&& /* wheelchairArg */,
      std::optional<int>&& /* numItinerariesArg */,
      std::optional<gql::response::Value>&& /* searchWindowArg */,
      std::optional<std::string>&& /* pageCursorArg */,
      std::optional<double>&& /* bikeReluctanceArg */,
      std::optional<double>&& /* bikeWalkingReluctanceArg */,
      std::optional<double>&& /* carReluctanceArg */,
      std::optional<double>&& /* walkReluctanceArg */,
      std::optional<double>&& /* waitReluctanceArg */,
      std::optional<double>&& /* walkSpeedArg */,
      std::optional<double>&& /* bikeSpeedArg */,
      std::optional<int>&& /* bikeSwitchTimeArg */,
      std::optional<int>&& /* bikeSwitchCostArg */,
      std::optional<otp::OptimizeType>&& /* optimizeArg */,
      std::unique_ptr<otp::InputTriangle>&& /* triangleArg */,
      std::optional<bool>&& /* arriveByArg */,
      std::unique_ptr<otp::InputPreferred>&& /* preferredArg */,
      std::unique_ptr<otp::InputUnpreferred>&& /* unpreferredArg */,
      std::optional<int>&& /* walkBoardCostArg */,
      std::optional<int>&& /* bikeBoardCostArg */,
      std::unique_ptr<otp::InputBanned>&& /* bannedArg */,
      std::optional<int>&& /* transferPenaltyArg */,
      std::optional<std::vector<std::unique_ptr<otp::TransportMode>>>&& /*
          transportModesArg */
      ,
      std::unique_ptr<otp::InputModeWeight>&& /* modeWeightArg */,
      std::optional<bool>&& /* debugItineraryFilterArg */,
      std::optional<bool>&& /* allowKeepingRentedBicycleAtDestinationArg */,
      std::optional<int>&& /* keepingRentedBicycleAtDestinationCostArg */,
      std::optional<int>&& /* boardSlackArg */,
      std::optional<int>&& /* alightSlackArg */,
      std::optional<int>&& /* minTransferTimeArg */,
      std::optional<int>&& /* nonpreferredTransferPenaltyArg */,
      std::optional<int>&& /* maxTransfersArg */,
      std::optional<std::string>&& /* startTransitStopIdArg */,
      std::optional<bool>&& /* omitCanceledArg */,
      std::optional<bool>&& /* ignoreRealtimeUpdatesArg */,
      std::optional<std::string>&& /* localeArg */,
      std::optional<std::vector<std::optional<std::string>>>&& /*
          allowedTicketTypesArg */
      ,
      std::optional<std::vector<std::optional<std::string>>>&& /*
          allowedVehicleRentalNetworksArg */
      ,
      std::optional<std::vector<std::optional<std::string>>>&& /*
          bannedVehicleRentalNetworksArg */
      ,
      std::optional<double>&& /* walkSafetyFactorArg */,
      std::unique_ptr<otp::VehicleParkingInput>&& /* parkingArg */,
      std::optional<double>&& /* maxWalkDistanceArg */,
      std::optional<double>&& /* walkOnStreetReluctanceArg */,
      std::optional<double>&& /* waitAtBeginningFactorArg */,
      std::optional<bool>&& /* batchArg */,
      std::optional<bool>&& /* allowBikeRentalArg */,
      std::optional<gql::response::Value>&& /* claimInitialWaitArg */,
      std::optional<bool>&& /* reverseOptimizeOnTheFlyArg */,
      std::optional<bool>&& /* disableRemainingWeightHeuristicArg */,
      std::optional<bool>&& /* compactLegsByReversedSearchArg */,
      std::optional<std::vector<std::optional<std::string>>>&& /*
          allowedBikeRentalNetworksArg */
      ,
      std::optional<int>&& /* maxPreTransitTimeArg */,
      std::optional<double>&& /* carParkCarLegWeightArg */,
      std::optional<int>&& /* heuristicStepsPerMainStepArg */,
      std::optional<double>&& /* itineraryFilteringArg */,
      std::optional<std::vector<std::unique_ptr<otp::InputCoordinates>>>&& /*
          intermediatePlacesArg */
      ,
      std::optional<std::string>&& /* startTransitTripIdArg */) const noexcept {
    return std::make_shared<otpo::Plan>(std::make_shared<plan>());
  }
};

graphql::graphql() : module("GraphQL", "graphql") {}

void graphql::init(motis::module::registry& reg) {
  reg.register_op(
      "/graphql",
      [](mm::msg_ptr const& msg) {
        auto const req = motis_content(HTTPRequest, msg);

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
                           .operationName = "GetPlan"sv,
                           .variables = std::move(variables)})
                .get());

        mm::message_creator mc;
        mc.create_and_finish(
            MsgContent_HTTPResponse,
            CreateHTTPResponse(
                mc, HTTPStatus_OK,
                mc.CreateVector(std::vector<fbb::Offset<HTTPHeader>>{}),
                mc.CreateString(response))
                .Union());
        return make_msg(mc);
      },
      {});
}

}  // namespace motis::graphql