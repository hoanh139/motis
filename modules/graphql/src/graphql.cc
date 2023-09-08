#include "motis/graphql/graphql.h"

#include "motis/core/common/date_time_util.h"
#include "motis/core/journey/journey.h"
#include "motis/core/journey/message_to_journeys.h"

#include "motis/module/context/motis_call.h"
#include "graphqlservice/JSONResponse.h"

#include "otp/ItineraryObject.h"
#include "otp/LegObject.h"
#include "otp/PlaceObject.h"
#include "otp/PlanObject.h"
#include "otp/QueryTypeObject.h"
#include "otp/StopObject.h"
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

struct pattern {
  gql::response::IdType getId() const noexcept { return id_; };
  std::shared_ptr<otpo::Route> getRoute() const noexcept { return route_; };
  std::string getCode() const noexcept { return code_; };
  std::optional<std::vector<std::shared_ptr<Stop>>> getStops() const noexcept {
    return stops_;
  };
  std::optional<std::vector<std::shared_ptr<otpo::Coordinates>>> getGeometry()
      const noexcept {
    return geometry_;
  };

  gql::response::IdType id_;
  std::shared_ptr<otpo::Route> route_;
  std::string code_;
  std::optional<std::vector<std::shared_ptr<Stop>>> stops_;
  std::optional<std::vector<std::shared_ptr<otpo::Coordinates>>> geometry_;
};

struct stoptime {
  std::shared_ptr<Stop> getStop() const noexcept { return stop_; };
  std::optional<otp::RealtimeState> getRealtimeState() const noexcept {
    return realtime_state;
  };
  std::optional<otp::PickupDropoffType> getPickupType() const noexcept {
    return pickup_type;
  };

  std::shared_ptr<Stop> stop_;
  std::optional<otp::RealtimeState> realtime_state;
  std::optional<otp::PickupDropoffType> pickup_type;
};

struct agency {
  explicit agency(gql::response::IdType&& idArg, std::string&& gtfsIdArg,
                  std::string&& nameArg, std::string&& urlArg) {
    id_ = std::move(idArg);
    gtfs_id = gtfsIdArg;
    name_ = nameArg;
    url_ = urlArg;
  }

  gql::response::IdType getId() const noexcept { return id_; };
  std::string getGtfsId() const noexcept { return gtfs_id; };
  std::string getName() const noexcept { return name_; };
  std::string getUrl() const noexcept { return url_; };
  std::optional<std::string> getPhone() const noexcept { return phone_; };
  std::optional<std::string> getFareUrl() const noexcept { return fare_url; };

  gql::response::IdType id_;
  std::string gtfs_id;
  std::string name_;
  std::string url_;
  std::optional<std::string> phone_;
  std::optional<std::string> fare_url;
};

struct trip {
  gql::response::IdType getId() const noexcept { return id_; };
  std::string getGtfsId() const noexcept { return gtfs_id; };
  std::optional<std::string> getTripHeadsign() const noexcept {
    return trip_headsign;
  };
  std::optional<std::string> getDirectionId() const noexcept {
    return direction_id;
  };
  std::shared_ptr<otpo::Pattern> getPattern() const noexcept {
    return pattern_;
  };
  std::optional<std::vector<std::shared_ptr<otpo::Stoptime>>> getStoptimes()
      const noexcept {
    return stoptimes_;
  };
  std::optional<std::vector<std::shared_ptr<otpo::Stoptime>>>
  getStoptimesForDate() const noexcept {
    return stoptimes_for_date;
  };

  gql::response::IdType id_;
  std::string gtfs_id;
  std::optional<std::string> trip_headsign;
  std::optional<std::string> direction_id;
  std::shared_ptr<otpo::Pattern> pattern_;
  std::optional<std::vector<std::shared_ptr<otpo::Stoptime>>> stoptimes_;
  std::optional<std::vector<std::shared_ptr<otpo::Stoptime>>>
      stoptimes_for_date;
};

struct route {
  gql::response::IdType getId() const noexcept { return id_; };
  std::string getGtfsId() const noexcept { return gtfs_id; };
  std::shared_ptr<otpo::Agency> getAgency() const noexcept { return agency_; };
  std::optional<std::string> getShortName() const noexcept {
    return short_name;
  };
  std::optional<std::string> getLongName() const noexcept { return long_name; };
  std::optional<otp::TransitMode> getMode() const noexcept { return mode_; };
  std::optional<int> getType() const noexcept { return type_; };
  std::optional<std::string> getDesc() const noexcept { return desc_; };
  std::optional<std::string> getUrl() const noexcept { return url_; };
  std::optional<std::string> getColor() const noexcept { return color_; };
  std::optional<std::vector<std::shared_ptr<otpo::Alert>>> getAlerts()
      const noexcept {
    return alerts_;
  };

  gql::response::IdType id_;
  std::string gtfs_id;
  std::shared_ptr<otpo::Agency> agency_;
  std::optional<std::string> short_name;
  std::optional<std::string> long_name;
  std::optional<otp::TransitMode> mode_;
  std::optional<int> type_;
  std::optional<std::string> desc_;
  std::optional<std::string> url_;
  std::optional<std::string> color_;
  std::optional<std::vector<std::shared_ptr<otpo::Alert>>> alerts_;
};

struct stop {
  explicit stop(gql::response::IdType&& idArg, std::string&& gtfsIdArg,
                std::string&& nameArg, std::optional<double>&& latArg,
                std::optional<double>&& lonArg) {
    id_ = std::move(idArg);
    gtfs_id = gtfsIdArg;
    name_ = nameArg;
    lat_ = latArg;
    lon_ = lonArg;
  }

  gql::response::IdType getId() const noexcept { return id_; };
  std::string getGtfsId() const noexcept { return gtfs_id; };
  std::string getName() const noexcept { return name_; };
  std::optional<double> getLat() const noexcept { return lat_; };
  std::optional<double> getLon() const noexcept { return lon_; };
  std::optional<std::string> getCode() const noexcept { return code; };
  std::optional<std::string> getZoneId() const noexcept { return zone_id; };
  std::optional<std::string> getPlatformCode() const noexcept {
    return platform_code;
  };

  gql::response::IdType id_;
  std::string gtfs_id;
  std::string name_;
  std::optional<double> lat_;
  std::optional<double> lon_;
  std::optional<std::string> code;
  std::optional<std::string> zone_id;
  std::optional<std::string> platform_code;
};

struct place {
  explicit place(double latArg, double lonArg,
                 std::optional<std::string>&& nameArg,
                 otp::VertexType&& vertexTypeArg,
                 gql::response::Value&& arrivalTimeArg,
                 std::shared_ptr<otpo::Stop>&& stopArg) {
    lat_ = latArg;
    lon_ = lonArg;
    name_ = nameArg;
    vertex_type = vertexTypeArg;
    arrival_time = std::move(arrivalTimeArg);
    stop_ = stopArg;
  }
  std::optional<std::string> getName() const noexcept { return name_; };
  std::optional<otp::VertexType> getVertexType() const noexcept {
    return vertex_type;
  };
  double getLat() const noexcept { return lat_; }
  double getLon() const noexcept { return lon_; }
  gql::response::Value getArrivalTime() const noexcept {
    return gql::response::Value{arrival_time};
  }
  std::shared_ptr<otpo::Stop> getStop() const noexcept { return stop_; };
  std::shared_ptr<otpo::BikeRentalStation> getBikeRentalStation()
      const noexcept {
    return bike_rental_station;
  };
  std::shared_ptr<otpo::BikePark> getBikePark() const noexcept {
    return bike_park;
  };
  std::shared_ptr<otpo::VehicleParking> getVehicleParking() const noexcept {
    return vehicle_parking;
  };
  double lat_;
  double lon_;
  gql::response::Value arrival_time;
  std::optional<std::string> name_;
  std::optional<otp::VertexType> vertex_type;
  std::shared_ptr<otpo::Stop> stop_;
  std::shared_ptr<otpo::BikeRentalStation> bike_rental_station;
  std::shared_ptr<otpo::BikePark> bike_park;
  std::shared_ptr<otpo::VehicleParking> vehicle_parking;
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
  explicit leg(const std::optional<otp::Mode>&& modeArg,
               const std::optional<std::vector<std::shared_ptr<otpo::Alert>>>&&
                   alertsArg,
               const std::shared_ptr<otpo::Agency>&& agencyArg,
               const std::shared_ptr<otpo::Place>&& fromArg,
               const std::shared_ptr<otpo::Place>&& toArg,
               const std::shared_ptr<otpo::Geometry>&& legGeometryArg,
               const std::optional<std::vector<std::shared_ptr<otpo::Place>>>&&
                   intermediatePlacesArg,
               const std::optional<bool>&& realtimeArg,
               const std::optional<bool>&& transitLegArg, int startTimeArg,
               const std::optional<int>&& departureDelayArg,
               const std::optional<int>&& arrivalDelayArg,
               const std::optional<bool>&& interlineWithPreviousLegArg,
               const std::optional<double>&& distanceArg,
               const std::optional<double>&& durationArg,
               const std::optional<bool>&& intermediatePlaceArg,
               const std::shared_ptr<otpo::Route>&& routeArg,
               const std::shared_ptr<otpo::Trip>&& tripArg) {
    mode_ = modeArg;
    alerts_ = alertsArg;
    agency_ = std::move(agencyArg);
    from_ = fromArg;
    to_ = toArg;
    leg_geometry = legGeometryArg;
    intermediate_places = std::move(intermediatePlacesArg);
    realtime_ = realtimeArg;
    transit_leg = transitLegArg;
    start_time = gql::response::Value{startTimeArg};
    departure_delay = departureDelayArg;
    arrival_delay = arrivalDelayArg;
    interline_with_previous_leg = interlineWithPreviousLegArg;
    distance_ = distanceArg;
    duration_ = durationArg;
    intermediate_place = intermediatePlaceArg;
    route_ = routeArg;
    trip_ = tripArg;
  }

  std::optional<gql::response::Value> getStartTime() const noexcept {
    return start_time;
  };
  std::optional<int> getDepartureDelay() const noexcept {
    return departure_delay;
  };
  std::optional<int> getArrivalDelay() const noexcept { return arrival_delay; };
  std::optional<otp::Mode> getMode() const noexcept { return mode_; };
  std::optional<double> getDuration() const noexcept { return duration_; };
  std::shared_ptr<otpo::Geometry> getLegGeometry() const noexcept {
    return leg_geometry;
  };
  std::shared_ptr<otpo::Agency> getAgency() const noexcept { return agency_; };
  std::optional<bool> getRealTime() const noexcept { return realtime_; };
  std::optional<otp::RealtimeState> getRealtimeState() const noexcept {
    return realtime_state;
  };
  std::optional<double> getDistance() const noexcept { return distance_; };
  std::optional<bool> getTransitLeg() const noexcept { return transit_leg; };
  std::optional<bool> getRentedBike() const noexcept { return rented_bike; };
  std::shared_ptr<otpo::Place> getFrom() const noexcept { return from_; };
  std::shared_ptr<otpo::Place> getTo() const noexcept { return to_; };
  std::shared_ptr<otpo::Route> getRoute() const noexcept { return route_; };
  std::shared_ptr<otpo::Trip> getTrip() const noexcept { return trip_; };
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
    return alerts_;
  };

  std::optional<otp::Mode> mode_;
  std::optional<std::vector<std::shared_ptr<otpo::Alert>>> alerts_;
  std::shared_ptr<otpo::Agency> agency_;
  std::shared_ptr<otpo::Place> from_;
  std::shared_ptr<otpo::Place> to_;
  std::shared_ptr<otpo::BookingInfo> drop_off_booking_info;
  std::shared_ptr<otpo::Geometry> leg_geometry;
  std::optional<std::vector<std::shared_ptr<otpo::Place>>> intermediate_places;
  std::optional<bool> realtime_;
  std::optional<otp::RealtimeState> realtime_state;
  std::optional<bool> transit_leg;
  std::optional<bool> rented_bike;
  std::optional<gql::response::Value> start_time;
  std::optional<int> departure_delay;
  std::optional<int> arrival_delay;
  std::optional<bool> interline_with_previous_leg;
  std::optional<double> distance_;
  std::optional<double> duration_;
  std::optional<bool> intermediate_place;
  std::shared_ptr<otpo::Route> route_;
  std::shared_ptr<otpo::Trip> trip_;
};

struct itinerary {
  //  explicit itinerary(gql::response::Value startTimeArg,
  //                     gql::response::Value endTimeArg,
  //                     gql::response::Value durationArg, double walkDistArg,
  //                     std::vector<std::shared_ptr<otpo::Leg>> legsArg,
  //                     std::vector<std::shared_ptr<otpo::fare>> faresArg) {
  //    start_time = std::move(startTimeArg);
  //    end_time = std::move(endTimeArg);
  //    duration_ = std::move(durationArg);
  //    legs_ = std::move(legsArg);
  //    fares_ = std::move(faresArg);
  //  };
  explicit itinerary(int startTimeArg, int endTimeArg, int durationArg,
                     double walkDistArg,
                     std::vector<std::shared_ptr<otpo::Leg>> legsArg) {
    start_time = gql::response::Value{startTimeArg};
    end_time = gql::response::Value{endTimeArg};
    duration_ = gql::response::Value{durationArg};
    legs_ = std::move(legsArg);
  };
  std::optional<gql::response::Value> getStartTime() const noexcept {
    return start_time;
  };
  std::optional<gql::response::Value> getEndTime() const noexcept {
    return end_time;
  };
  std::optional<gql::response::Value> getDuration() const noexcept {
    return duration_;
  };
  std::optional<double> getWalkDistance() const noexcept {
    return walk_distance;
  };
  std::vector<std::shared_ptr<otpo::Leg>> getLegs() const noexcept {
    return legs_;
  };
  std::optional<std::vector<std::shared_ptr<otpo::fare>>> getFares()
      const noexcept {
    return fares_;
  };

  std::optional<gql::response::Value> start_time;
  std::optional<gql::response::Value> end_time;
  std::optional<gql::response::Value> duration_;
  std::optional<double> walk_distance;
  std::optional<bool> arrived_at_dest_with_rented_bicycle;

  std::vector<std::shared_ptr<otpo::Leg>> legs_;
  std::optional<std::vector<std::shared_ptr<otpo::fare>>> fares_;
};

// Begin

otp::Mode getModeFromStation(const journey::transport& tran) {
  if (tran.is_walk_) {
    return otp::Mode::WALK;
  }
  otp::Mode vehicleMode;
  switch (tran.clasz_) {
    case 0: {
      vehicleMode = otp::Mode::AIRPLANE;
      break;
    }
    case 1:
    case 2:
    case 4:
    case 5:
    case 6:
    case 7: vehicleMode = otp::Mode::RAIL; break;
    case 3:
    case 10: vehicleMode = otp::Mode::BUS; break;
    case 8: vehicleMode = otp::Mode::SUBWAY; break;
    case 9: vehicleMode = otp::Mode::TRAM; break;
    case 11: vehicleMode = otp::Mode::FERRY; break;
    default: vehicleMode = otp::Mode::TRANSIT;
  }
  return vehicleMode;
}

std::shared_ptr<otpo::Place> CreatePlaceWithTransport(
    const journey::stop& stopArg, const journey::transport& transport) {
  double lat = stopArg.lat_;
  double lon = stopArg.lng_;

  std::string stopName = stopArg.name_;

  auto const arrivalTime = stopArg.arrival_.timestamp_;
  const std::shared_ptr<otpo::BikePark> bikePark = nullptr;
  // bikeRentalStation

  // CarPark - To
  // vehicleParkingWithEntrance -To

  otp::VertexType vertexTypeFrom;
  std::shared_ptr<otpo::Stop> stop_place = nullptr;
  if (stopName == "START" || stopName == "END") {
    vertexTypeFrom = otp::VertexType::NORMAL;
  } else {
    vertexTypeFrom = otp::VertexType::TRANSIT;

    // Create stop
    auto stopID =
        gql::response::IdType{gql::response::StringType{stopArg.eva_no_}};
    // gtfsId ?? FeedID
    std::string gtfsId = "";
    // code
    // plattformCode
    // zoneID
    otp::Mode vehicleMode = getModeFromStation(transport);
    stop_place = std::make_shared<otpo::Stop>(std::make_shared<stop>(
        std::move(stopID), std::move(gtfsId), std::move(stopName), lat, lon));
  }
  return std::make_shared<otpo::Place>(
      std::make_shared<place>(lat, lon, stopName, std::move(vertexTypeFrom),
                              gql::response::Value{1}, std::move(stop_place)));
}

std::shared_ptr<otpo::Itinerary> createItinerary(const Connection* con) {
  auto const journey = motis::convert(con);

  // create other infos
  auto const start_time_Itinerary =
      journey.stops_.begin()->departure_.timestamp_;
  auto const end_time_Itinerary = (journey.stops_.end()--)->arrival_.timestamp_;
  auto const duration_Itinerary = start_time_Itinerary - end_time_Itinerary;
  // walk_distance_Itinerary ??

  // create legs_Itinerary
  std::vector<std::shared_ptr<otpo::Leg>> legs;
  for (auto const& tran : journey.transports_) {

    auto const mode = getModeFromStation(tran);

    auto const stop_from = journey.stops_.at(tran.from_);
    auto const stop_to = journey.stops_.at(tran.to_);

    auto const start_time_leg = stop_from.departure_.timestamp_;
    auto const end_time_leg = stop_to.arrival_.timestamp_;

    auto const departure_delay = stop_from.departure_.timestamp_ -
                                 stop_from.departure_.schedule_timestamp_;
    auto const arrival_delay =
        stop_from.arrival_.timestamp_ - stop_from.arrival_.schedule_timestamp_;

    auto const duration_leg = tran.duration_;

    // legGeometry

    // Create Agency
    std::shared_ptr<otpo::Agency> agc = nullptr;
    if (!tran.is_walk_) {
      auto agency_id =
          gql::response::IdType(gql::response::StringType{"unknown"});
      std::string agency_gtfs_id = "1";
      std::string agency_name = "1";
      std::string agency_url = "1";
      //      auto tryy = std::make_shared<agency>(
      //          std::move(agency_id), std::move(agency_gtfs_id),
      //          std::move(agency_name), std::move(agency_url));
      //      agc = std::make_shared<otpo::Agency>();
    }

    auto const real_time = false;  //??
    // realTimeState ??
    auto const distance_ = 0;  // Can't be determined

    bool transit_leg = true;
    if (stop_from.name_ == "START" || stop_to.name_ == "END") {
      transit_leg = false;
    }

    auto const rented_bike = false;  // ??

    auto const from =
        CreatePlaceWithTransport(journey.stops_.at(tran.from_), tran);

    auto const to = CreatePlaceWithTransport(journey.stops_.at(tran.to_), tran);

    // route - später
    // trip  - später

    std::vector<std::shared_ptr<otpo::Place>> intermediatePlaces;
    if (!tran.is_walk_) {
      for (size_t count = tran.from_ + 1; count < tran.to_; count++) {
        intermediatePlaces.push_back(
            CreatePlaceWithTransport(journey.stops_.at(count), tran));
      }
    }
    bool intermediatePlace = false;  // ??
    bool interlineWithPreviousLeg = false;  // ??
    // dropOffBookingInfo - immer null ??

    const std::vector<std::shared_ptr<otpo::Alert>> alerts =
        std::vector<std::shared_ptr<otpo::Alert>>();  // ??

    auto const l = std::make_shared<leg>(
        mode, alerts, std::move(nullptr), std::move(from), std::move(to),
        std::move(nullptr), std::move(intermediatePlaces), real_time,
        transit_leg, start_time_leg, departure_delay, arrival_delay,
        interlineWithPreviousLeg, distance_, duration_leg, intermediatePlace,
        std::move(nullptr), std::move(nullptr));
    auto const otpLeg = std::make_shared<otpo::Leg>(l);

    legs.push_back(otpLeg);
  }

  return std::make_shared<otpo::Itinerary>(
      std::make_shared<itinerary>(start_time_Itinerary, end_time_Itinerary,
                                  duration_Itinerary, 0, std::move(legs)));
}

std::tuple<std::string, double, double> convertStringToCoordinate(
    const std::string& arg) {
  std::stringstream ss(arg);
  std::string str;

  getline(ss, str, ':');
  auto const address_ = str;
  getline(ss, str, ':');

  getline(ss, str, ',');
  //  std::cout << str << std::endl;
  auto const lat_ = std::stod(str);
  getline(ss, str, ',');
  //  std::cout << str << std::endl;
  auto const lon_ = std::stod(str);

  return std::make_tuple(address_, lat_, lon_);
}

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
                std::optional<std::string>&& fromPlaceArg,
                std::optional<std::string>&& toPlaceArg,
                std::optional<int>&& numItinerariesArg) {

    // Create Intermodal Routing Request
    mm::message_creator mc;

    auto const begin = convertTime(dateArg.value(), timeArg.value());
    auto const end = convertTime(dateArg.value(), timeArg.value(), true);

    auto const fromPos = convertStringToCoordinate(fromPlaceArg.value());
    auto const toPos = convertStringToCoordinate(toPlaceArg.value());

    auto const interval = Interval(begin, end);
    //    auto const interval = Interval(1692869160, 1692876360);
    auto const start_position =
        motis::Position{std::get<1>(fromPos), std::get<2>(fromPos)};
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
    auto dest = intermodal::CreateInputPosition(mc, std::get<1>(toPos),
                                                std::get<2>(toPos))
                    .Union();
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
    date_ = gql::response::Value{begin};

    // itinerary init
    auto const intermodal_res = motis_content(RoutingResponse, res_value);
    for (auto const c : *intermodal_res->connections()) {
      auto const itinerary = createItinerary(c);
      itineraries_.push_back(itinerary);
    }
  }

  gql::response::Value getDate() const noexcept {
    return gql::response::Value{date_};
  }
  std::shared_ptr<otpo::Place> getFrom() const noexcept { return from_; }
  std::shared_ptr<otpo::Place> getTo() const noexcept { return to_; }
  std::vector<std::shared_ptr<otpo::Itinerary>> getItineraries()
      const noexcept {
    return itineraries_;
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
  gql::response::Value date_;
  std::shared_ptr<otpo::Place> from_;
  std::shared_ptr<otpo::Place> to_;

  std::vector<std::shared_ptr<otpo::Itinerary>> itineraries_;
};

struct Query {
  std::shared_ptr<otpo::Plan> getPlan(
      std::optional<std::string>&& dateArg,
      std::optional<std::string>&& timeArg,
      std::unique_ptr<otp::InputCoordinates>&& fromArg,
      std::unique_ptr<otp::InputCoordinates>&& toArg,
      std::optional<std::string>&& fromPlaceArg,
      std::optional<std::string>&& toPlaceArg,
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
        std::move(dateArg), std::move(timeArg), std::move(fromPlaceArg),
        std::move(toPlaceArg), std::move(numItinerariesArg)));
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