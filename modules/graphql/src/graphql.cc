#include "motis/graphql/graphql.h"

#include "motis/module/context/motis_call.h"
#include "graphqlservice/JSONResponse.h"

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
using namespace motis::routing;

namespace motis::graphql {

using String = gql::response::StringType;
using Float = gql::response::FloatType;
using Boolean = gql::response::BooleanType;
using Long = gql::response::IntType;
using Int = gql::response::IntType;

struct place {
  explicit place(std::unique_ptr<otp::InputCoordinates>&& coord) {
    _lon = coord->lon;
    _lat = coord->lat;
    _stop = nullptr;
  };
  std::optional<std::string> getName() const noexcept { return _name; };
  std::optional<otp::VertexType> getVertexType() const noexcept {
    return vertex_type;
  };
  double getLat() const noexcept { return _lat; }
  double getLon() const noexcept { return _lon; }
  std::shared_ptr<otpo::Stop> getStop() const noexcept { return _stop; };
  std::shared_ptr<otpo::BikeRentalStation> getBikeRentalStation()
      const noexcept {
    return bike_rental_station;
  };
  std::shared_ptr<otpo::BikePark> getBikePark() const noexcept {
    return bike_park;
  };
  double _lat;
  double _lon;
  std::optional<std::string> _name;
  std::optional<otp::VertexType> vertex_type;
  std::shared_ptr<otpo::Stop> _stop;
  std::shared_ptr<otpo::BikeRentalStation> bike_rental_station;
  std::shared_ptr<otpo::BikePark> bike_park;
};

struct geometrie {
  std::optional<int> getLength() const noexcept { return _length; };
  std::optional<gql::response::Value> getPoints() const noexcept {
    return _points;
  };
  std::optional<int> _length;
  std::optional<gql::response::Value> _points;
};

struct leg {
  std::optional<gql::response::Value> getStartTime() const noexcept {
    return start_time;
  };
  std::optional<int> getDepartureDelay() const noexcept {
    return departure_delay;
  };
  std::optional<int> getArrivalDelay() const noexcept { return arrival_delay; };
  std::optional<otp::Mode> getMode() const noexcept { return _mode; };
  std::optional<double> getDuration() const noexcept { return _duration; };
  std::shared_ptr<otpo::Geometry> getLegGeometry() const noexcept {
    return leg_geometry;
  };
  std::shared_ptr<otpo::Agency> getAgency() const noexcept { return _agency; };
  std::optional<bool> getRealTime() const noexcept { return _realtime; };
  std::optional<otp::RealtimeState> getRealtimeState() const noexcept {
    return realtime_state;
  };
  std::optional<double> getDistance() const noexcept { return _distance; };
  std::optional<bool> getTransitLeg() const noexcept { return transit_leg; };
  std::optional<bool> getRentedBike() const noexcept { return rented_bike; };
  std::shared_ptr<otpo::Place> getFrom() const noexcept { return _from; };
  std::shared_ptr<otpo::Place> getTo() const noexcept { return _to; };
  std::shared_ptr<otpo::Route> getRoute() const noexcept { return _route; };
  std::shared_ptr<Trip> getTrip() const noexcept { return _trip; };
  std::optional<std::vector<std::shared_ptr<otpo::Place>>>
  getIntermediatePlaces() const noexcept {
    return intermediate_places;
  };
  std::optional<bool> getIntermediatePlace() const noexcept {
    return intermediate_place;
  };
  std::optional<bool> getInterlineWithPreviousLeg() const noexcept {
    return interline_with_previous_leg;
  };
  std::shared_ptr<otpo::BookingInfo> getDropOffBookingInfo() const noexcept {
    return drop_off_booking_info;
  };
  std::optional<std::vector<std::shared_ptr<otpo::Alert>>> getAlerts()
      const noexcept {
    return _alerts;
  };

  std::optional<otp::Mode> _mode;
  std::optional<std::vector<std::shared_ptr<otpo::Alert>>> _alerts;
  std::shared_ptr<otpo::Agency> _agency;
  std::shared_ptr<otpo::Place> _from;
  std::shared_ptr<otpo::Place> _to;
  std::shared_ptr<otpo::BookingInfo> drop_off_booking_info;
  std::shared_ptr<otpo::Geometry> leg_geometry;
  std::optional<std::vector<std::shared_ptr<otpo::Place>>> intermediate_places;
  std::optional<bool> _realtime;
  std::optional<otp::RealtimeState> realtime_state;
  std::optional<bool> transit_leg;
  std::optional<bool> rented_bike;
  std::optional<gql::response::Value> start_time;
  std::optional<int> departure_delay;
  std::optional<int> arrival_delay;
  std::optional<bool> interline_with_previous_leg;
  std::optional<double> _distance;
  std::optional<double> _duration;
  std::optional<bool> intermediate_place;
  std::shared_ptr<otpo::Route> _route;
  std::shared_ptr<Trip> _trip;
};

struct itinerary {
  std::optional<gql::response::Value> getStartTime() const noexcept {
    return start_time;
  };
  std::optional<gql::response::Value> getEndTime() const noexcept {
    return end_time;
  };
  std::optional<gql::response::Value> getDuration() const noexcept {
    return _duration;
  };
  std::optional<double> getWalkDistance() const noexcept {
    return walk_distance;
  };
  std::vector<std::shared_ptr<otpo::Leg>> getLegs() const noexcept {
    return _legs;
  };
  std::optional<std::vector<std::shared_ptr<otpo::fare>>> getFares()
      const noexcept {
    return _fares;
  };

  std::optional<gql::response::Value> start_time;
  std::optional<gql::response::Value> end_time;
  std::optional<gql::response::Value> _duration;
  std::optional<double> walk_distance;
  std::optional<bool> arrived_at_dest_with_rented_bicycle;

  std::vector<std::shared_ptr<otpo::Leg>> _legs;
  std::optional<std::vector<std::shared_ptr<otpo::fare>>> _fares;
};

struct plan {
  explicit plan() {
    // date_arg = gql::response::Value{"a"};
    from_arg = nullptr;
    to_arg = nullptr;
  }

  explicit plan(std::optional<std::string>&& dateArg,
                std::unique_ptr<otp::InputCoordinates>&& fromArg,
                std::unique_ptr<otp::InputCoordinates>&& toArg) {

    date_arg = gql::response::Value{dateArg.value_or("unknown")};
    from_arg = std::make_shared<otpo::Place>(
        std::make_shared<place>(std::move(fromArg)));
    to_arg = std::make_shared<otpo::Place>(
        std::make_shared<place>(std::move(toArg)));

    mm::message_creator mc;
    mc.create_and_finish(
        MsgContent_IntermodalRoutingRequest,
        intermodal::CreateIntermodalRoutingRequest(
            mc, intermodal::IntermodalStart_IntermodalPretripStart,
            //            flatbuffers::Offset<void>{intermodal::CreateIntermodalPretripStart(
            //                mc, new motis::Position(1.0, 2.0), new
            //                motis::Interval(1, 3), 0, false, false)},
            flatbuffers::Offset<void>(1),
            mc.CreateVector(std::vector<flatbuffers::Offset<
                                motis::intermodal::ModeWrapper>>{
                intermodal::CreateModeWrapper(mc, intermodal::Mode::Mode_Foot,
                                              flatbuffers::Offset<void>())}),
            intermodal::IntermodalDestination::
                IntermodalDestination_InputPosition,
            flatbuffers::Offset<void>(2))
            .Union(),
        "/intermodal");

    auto const response = motis_call(make_msg(mc));
    response->val();  // msg_ptr
    auto const res_value = response->val();
    auto const intermodal_res = motis_content(RoutingResponse, res_value);
    for (auto const c : *intermodal_res->connections()) {
      auto tryyy = c;
    }
  }

  gql::response::Value getDate() const noexcept {
    return gql::response::Value{"a"};
  }
  std::shared_ptr<otpo::Place> getFrom() const noexcept { return from_arg; }
  std::shared_ptr<otpo::Place> getTo() const noexcept { return to_arg; }
  std::vector<std::shared_ptr<otpo::Itinerary>> getItineraries()
      const noexcept {
    return _itineraries;
  }
  //  std::vector<std::shared_ptr<gql::response::StringType>> getMessageEnums()
  // =={
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
  gql::response::Value date_arg;
  std::shared_ptr<otpo::Place> from_arg;
  std::shared_ptr<otpo::Place> to_arg;

  std::vector<std::shared_ptr<otpo::Itinerary>> _itineraries;
};

struct Query {
  std::shared_ptr<otpo::Plan> getPlan(
      std::optional<std::string>&& dateArg,
      std::optional<std::string>&& /*  timeArg */,
      std::unique_ptr<otp::InputCoordinates>&& fromArg,
      std::unique_ptr<otp::InputCoordinates>&& toArg,
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
      std::optional<std::vector<
          std::unique_ptr<otp::TransportMode>>>&& /* transportModesArg */,
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
      std::optional<std::vector<
          std::optional<std::string>>>&& /* allowedTicketTypesArg */,
      std::optional<std::vector<
          std::optional<std::string>>>&& /* allowedVehicleRentalNetworksArg */,
      std::optional<std::vector<
          std::optional<std::string>>>&& /* bannedVehicleRentalNetworksArg */,
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
      std::optional<std::vector<
          std::optional<std::string>>>&& /* allowedBikeRentalNetworksArg */,
      std::optional<int>&& /* maxPreTransitTimeArg */,
      std::optional<double>&& /* carParkCarLegWeightArg */,
      std::optional<int>&& /* heuristicStepsPerMainStepArg */,
      std::optional<double>&& /* itineraryFilteringArg */,
      std::optional<std::vector<std::unique_ptr<
          otp::InputCoordinates>>>&& /* intermediatePlacesArg */,
      std::optional<std::string>&& /* startTransitTripIdArg */) const noexcept {
    return std::make_shared<otpo::Plan>(std::make_shared<plan>(
        std::move(dateArg), std::move(fromArg), std::move(toArg)));
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