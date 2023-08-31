#include "motis/graphql/graphql.h"

#include "motis/core/common/date_time_util.h"
#include "motis/core/journey/journey.h"
#include "motis/core/journey/message_to_journeys.h"

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

template <typename T>
int ReturnCorrespondPosition(int pos, const std::vector<T>& vec) {
  for (size_t i = 0; i < vec.size(); i++)
    if (pos > vec[i].from_ && pos < vec[i].to_) {
      return i;
    }
  return -1;
}

struct station {
  motis::journey::stop stop_;
  motis::journey::transport transport_;
  motis::journey::trip trip_;
  motis::journey::ranged_attribute ranged_attribute;
};

struct place {
  explicit place(std::unique_ptr<otp::InputCoordinates>&& coord) {
    lat_ = coord->lat;
    lon_ = coord->lon;
    //    _stop = nullptr;
  };
  //  std::optional<std::string> getName() const noexcept { return _name; };
  //  std::optional<otp::VertexType> getVertexType() const noexcept {
  //    return vertex_type;
  //  };
  double getLat() const noexcept { return lat_; }
  double getLon() const noexcept { return lon_; }
  //    std::shared_ptr<otpo::Stop> getStop() const noexcept { return _stop; };
  //    std::shared_ptr<otpo::BikeRentalStation> getBikeRentalStation()
  //        const noexcept {
  //      return bike_rental_station;
  //    };
  //    std::shared_ptr<otpo::BikePark> getBikePark() const noexcept {
  //      return bike_park;
  //    };
  double lat_;
  double lon_;
  //  std::optional<std::string> _name;
  //  std::optional<otp::VertexType> vertex_type;
  //  std::shared_ptr<otpo::Stop> _stop;
  //  std::shared_ptr<otpo::BikeRentalStation> bike_rental_station;
  //  std::shared_ptr<otpo::BikePark> bike_park;
};

struct geometrie {
  //  std::optional<int> getLength() const noexcept { return _length; };
  //  std::optional<gql::response::Value> getPoints() const noexcept {
  //    return _points;
  //  };
  //  std::optional<int> _length;
  //  std::optional<gql::response::Value> _points;
};

struct leg {
  //  std::optional<gql::response::Value> getStartTime() const noexcept {
  //    return start_time;
  //  };
  //  std::optional<int> getDepartureDelay() const noexcept {
  //    return departure_delay;
  //  };
  //  std::optional<int> getArrivalDelay() const noexcept { return
  //  arrival_delay; }; std::optional<otp::Mode> getMode() const noexcept {
  //  return _mode; }; std::optional<double> getDuration() const noexcept {
  //  return _duration; }; std::shared_ptr<otpo::Geometry> getLegGeometry()
  //  const noexcept {
  //    return leg_geometry;
  //  };
  //  std::shared_ptr<otpo::Agency> getAgency() const noexcept { return _agency;
  //  }; std::optional<bool> getRealTime() const noexcept { return _realtime; };
  //  std::optional<otp::RealtimeState> getRealtimeState() const noexcept {
  //    return realtime_state;
  //  };
  //  std::optional<double> getDistance() const noexcept { return _distance; };
  //  std::optional<bool> getTransitLeg() const noexcept { return transit_leg;
  //  }; std::optional<bool> getRentedBike() const noexcept { return
  //  rented_bike; }; std::shared_ptr<otpo::Place> getFrom() const noexcept {
  //  return _from; }; std::shared_ptr<otpo::Place> getTo() const noexcept {
  //  return _to; }; std::shared_ptr<otpo::Route> getRoute() const noexcept {
  //  return _route; }; std::shared_ptr<Trip> getTrip() const noexcept { return
  //  _trip; }; std::optional<std::vector<std::shared_ptr<otpo::Place>>>
  //  getIntermediatePlaces() const noexcept {
  //    return intermediate_places;
  //  };
  //  std::optional<bool> getIntermediatePlace() const noexcept {
  //    return intermediate_place;
  //  };
  //  std::optional<bool> getInterlineWithPreviousLeg() const noexcept {
  //    return interline_with_previous_leg;
  //  };
  //  std::shared_ptr<otpo::BookingInfo> getDropOffBookingInfo() const noexcept
  //  {
  //    return drop_off_booking_info;
  //  };
  //  std::optional<std::vector<std::shared_ptr<otpo::Alert>>> getAlerts()
  //      const noexcept {
  //    return _alerts;
  //  };
  //
  //  std::optional<otp::Mode> _mode;
  //  std::optional<std::vector<std::shared_ptr<otpo::Alert>>> _alerts;
  //  std::shared_ptr<otpo::Agency> _agency;
  //  std::shared_ptr<otpo::Place> _from;
  //  std::shared_ptr<otpo::Place> _to;
  //  std::shared_ptr<otpo::BookingInfo> drop_off_booking_info;
  //  std::shared_ptr<otpo::Geometry> leg_geometry;
  //  std::optional<std::vector<std::shared_ptr<otpo::Place>>>
  //  intermediate_places; std::optional<bool> _realtime;
  //  std::optional<otp::RealtimeState> realtime_state;
  //  std::optional<bool> transit_leg;
  //  std::optional<bool> rented_bike;
  //  std::optional<gql::response::Value> start_time;
  //  std::optional<int> departure_delay;
  //  std::optional<int> arrival_delay;
  //  std::optional<bool> interline_with_previous_leg;
  //  std::optional<double> _distance;
  //  std::optional<double> _duration;
  //  std::optional<bool> intermediate_place;
  //  std::shared_ptr<otpo::Route> _route;
  //  std::shared_ptr<Trip> _trip;
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

int convertTime(const std::string& dateArg, const std::string& timeArg,
                bool extend = false) {
  std::string time_arg = timeArg;
  if (extend) {
    const char* format = "%H:%M:%S";
    std::tm result = {};
    std::stringstream ss(time_arg);
    if (strptime(time_arg.c_str(), format, &result) == nullptr) {
      throw std::runtime_error("strptime error");
    }
    result.tm_hour = result.tm_hour + 2;
    ss << std::put_time(&result, format);
    time_arg = ss.str();
  }

  auto date_str = dateArg + " " + time_arg + " CET";
  //  auto date_str = "2021-12-17 12:26:30 CET"; "2023-08-28 10:18:43 CET"
  auto const start_unix_time =
      motis::parse_unix_time(date_str, "%Y-%m-%d %H:%M:%S %Z");

  return start_unix_time;
};

const std::vector<station>& createListOfStations(const motis::journey& jn) {
  std::vector<station> stations;
  for (size_t pos = 0; pos < jn.stops_.size(); pos++) {
    station s;
    s.stop_ = jn.stops_.at(pos);
    if (int corr_pos = (ReturnCorrespondPosition(pos, jn.transports_)) != -1) {
      s.transport_ = jn.transports_.at(corr_pos);
    }
    if (int corr_pos = (ReturnCorrespondPosition(pos, jn.trips_)) != -1) {
      s.trip_ = jn.trips_.at(corr_pos);
    }
    if (int corr_pos = (ReturnCorrespondPosition(pos, jn.attributes_)) != -1) {
      s.ranged_attribute = jn.attributes_.at(corr_pos);
    }
    stations.push_back(s);
  }
  return stations;
}

void createItinerary(const Connection* con) {
  // create other infos
  auto const journey = motis::convert(con);
  auto const start_time = journey.stops_.begin()->departure_.timestamp_;
  auto const end_time = (journey.stops_.end()--)->arrival_.timestamp_;
  auto const duration = start_time - end_time;

  auto const station_list = createListOfStations(journey);

  // create legs
  for (auto const tran : journey.transports_) {
    auto const start_time_leg =
        journey.stops_.at(tran.from_).departure_.timestamp_;
    auto const end_time_leg = journey.stops_.at(tran.from_).arrival_.timestamp_;
    auto const departure_delay_leg = 0;
    auto const arrival_delay_leg = 0;
    auto const duration_leg = tran.duration_;
    // legGeometry
    // agency
    auto const real_time = false;
    // realTimeState
    auto const distance_ = 0;  // Can't be determined
    // auto const transis_leg = 0;
    // from
    // to
    // route
    // trip
    // intermediatePlaces
    // intermediatePlace
    // interlineWithPreviousLeg
    // dropOffBookingInfo
    // alerts

    if (tran.is_walk_) {
    } else {
    }
  }
}

struct plan {
  plan(std::optional<std::string>&& dateArg,
       std::optional<std::string>&& timeArg,
       std::unique_ptr<otp::InputCoordinates>&& fromArg,
       std::unique_ptr<otp::InputCoordinates>&& toArg,
       std::optional<std::string>&& fromPlaceArg,
       std::optional<std::string>&& toPlaceArg,
       std::optional<bool>&& wheelchairArg,
       std::optional<int>&& numItinerariesArg,
       std::optional<double>&& /* walkReluctanceArg */,
       std::optional<double>&& /* walkSpeedArg */,
       std::optional<double>&& /* bikeSpeedArg */,
       std::optional<otp::OptimizeType>&& /* optimizeArg */,
       std::unique_ptr<otp::InputTriangle>&& /* triangleArg */,
       std::optional<bool>&& /* arriveByArg */,
       std::unique_ptr<otp::InputUnpreferred>&& /* unpreferredArg */,
       std::optional<int>&& /* walkBoardCostArg */,
       std::optional<int>&& /* transferPenaltyArg */,
       std::optional<std::vector<
           std::unique_ptr<otp::TransportMode>>>&& /* transportModesArg */,
       std::optional<int>&& /* minTransferTimeArg */,
       std::optional<std::string>&& /* localeArg */,
       std::optional<std::vector<
           std::optional<std::string>>>&& /* allowedTicketTypesArg */,
       std::optional<std::vector<
           std::optional<std::string>>>&& /* allowedVehicleRentalNetworksArg */,
       std::optional<double>&& /* maxWalkDistanceArg */,
       std::optional<bool>&& /* disableRemainingWeightHeuristicArg */,
       std::optional<double>&& /* itineraryFilteringArg */,
       std::optional<std::vector<std::unique_ptr<
           otp::InputCoordinates>>>&& /* intermediatePlacesArg */) {}

  explicit plan(std::optional<std::string>&& dateArg,
                std::optional<std::string>&& timeArg,
                std::unique_ptr<otp::InputCoordinates>&& fromArg,
                std::unique_ptr<otp::InputCoordinates>&& toArg,
                std::optional<int>&& numItinerariesArg) {

    // Create Intermodal Routing Request
    mm::message_creator mc;

    auto const begin = convertTime(dateArg.value(), timeArg.value());
    auto const end = convertTime(dateArg.value(), timeArg.value(), true);

    auto const interval = Interval(begin, end);
    //    auto const interval = Interval(1692869160, 1692876360);
    auto const start_position = motis::Position{fromArg->lat, fromArg->lon};
    auto const imd_start = intermodal::CreateIntermodalPretripStart(
                               mc, &start_position, &interval,
                               numItinerariesArg.value_or(3), true, true)
                               .Union();
    auto const imd_start_modes = mc.CreateVector(
        std::vector<flatbuffers::Offset<motis::intermodal::ModeWrapper>>{
            intermodal::CreateModeWrapper(
                mc, intermodal::Mode::Mode_FootPPR,
                intermodal::CreateFootPPR(
                    mc, motis::ppr::CreateSearchOptions(
                            mc, mc.CreateString("default"), 900))
                    .Union())});
    auto dest =
        intermodal::CreateInputPosition(mc, toArg->lat, toArg->lon).Union();
    auto const dest_modes = mc.CreateVector(
        std::vector<flatbuffers::Offset<motis::intermodal::ModeWrapper>>{
            intermodal::CreateModeWrapper(
                mc, intermodal::Mode::Mode_FootPPR,
                intermodal::CreateFootPPR(
                    mc, motis::ppr::CreateSearchOptions(
                            mc, mc.CreateString("default"), 900))
                    .Union())});

    mc.create_and_finish(
        MsgContent_IntermodalRoutingRequest,
        intermodal::CreateIntermodalRoutingRequest(
            mc, intermodal::IntermodalStart_IntermodalPretripStart, imd_start,
            imd_start_modes,
            intermodal::IntermodalDestination::
                IntermodalDestination_InputPosition,
            dest, dest_modes, motis::routing::SearchType_Default,
            motis::SearchDir_Forward, mc.CreateString(""))
            .Union(),
        "/intermodal");
    // Response Handle
    auto const response = motis_call(make_msg(mc));
    auto const res_value = response->val();

    std::cout << res_value->to_json(mm::json_format::DEFAULT_FLATBUFFERS)
              << "\n";

    // date_arg init
    //    date_arg = gql::response::Value{begin};

    // Itineraries init
    auto const intermodal_res = motis_content(RoutingResponse, res_value);
    for (auto const c : *intermodal_res->connections()) {
      auto tryyyy = c;
      auto stops = c->stops();
      auto variable = c->attributes();
      auto lat = c->stops()->begin()->station()->pos()->lat();
      auto lng = c->stops()->begin()->station()->pos()->lng();

      auto const start_time = stops->begin()->departure()->time();
    }

    // try from and to
    from_arg = std::make_shared<otpo::Place>(
        std::make_shared<place>(std::move(fromArg)));
    to_arg = std::make_shared<otpo::Place>(
        std::make_shared<place>(std::move(toArg)));
  }

  gql::response::Value getDate() const noexcept {
    return gql::response::Value{"a"};
  }
  std::shared_ptr<otpo::Place> getFrom() const noexcept { return from_arg; }
  std::shared_ptr<otpo::Place> getTo() const noexcept { return to_arg; }
  //  std::vector<std::shared_ptr<otpo::Itinerary>> getItineraries()
  //      const noexcept {
  //    return _itineraries;
  //  }
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

  //  std::vector<std::shared_ptr<otpo::Itinerary>> _itineraries;
};

struct Query {
  std::shared_ptr<otpo::Plan> getPlan(
      std::optional<std::string>&& dateArg,
      std::optional<std::string>&& timeArg,
      std::unique_ptr<otp::InputCoordinates>&& fromArg,
      std::unique_ptr<otp::InputCoordinates>&& toArg,
      std::optional<std::string>&& /*  fromPlaceArg */,
      std::optional<std::string>&& /*  toPlaceArg */,
      std::optional<bool>&& /* wheelchairArg */,
      std::optional<int>&& numItinerariesArg,
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
        std::move(dateArg), std::move(timeArg), std::move(fromArg),
        std::move(toArg), std::move(numItinerariesArg)));
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