#include "motis/graphql/graphql.h"

#include "motis/core/common/date_time_util.h"
#include "motis/core/journey/journey.h"
#include "motis/core/journey/message_to_journeys.h"

#include "motis/core/access/time_access.h"
#include "motis/module/context/motis_call.h"
#include "motis/module/message.h"
#include "geo/polyline_format.h"
#include "graphqlservice/JSONResponse.h"

#include "otp/AgencyObject.h"
#include "otp/GeometryObject.h"
#include "otp/ItineraryObject.h"
#include "otp/LegObject.h"
#include "otp/PatternObject.h"
#include "otp/PlaceObject.h"
#include "otp/PlanObject.h"
#include "otp/QueryTypeObject.h"
#include "otp/RouteObject.h"
#include "otp/StopObject.h"
#include "otp/StoptimeObject.h"
#include "otp/TripObject.h"
#include "otp/debugOutputObject.h"
#include "otp/otpSchema.h"
#include "otp/serviceTimeRangeObject.h"

namespace mm = motis::module;
namespace fbb = flatbuffers;
namespace gql = graphql;
namespace otp = gql::otp;
namespace otpo = otp::object;

using motis::Connection;

using namespace std::string_view_literals;
using namespace motis::routing;
using namespace motis::osrm;
using namespace motis::ppr;
using namespace geo;

// geocoding

namespace motis::graphql {

using String = gql::response::StringType;
using Float = gql::response::FloatType;
using Boolean = gql::response::BooleanType;
using Long = gql::response::IntType;
using Int = gql::response::IntType;

struct service_time_range {
  explicit service_time_range(const int&& startArg, const int&& endArg) {
    start_ = gql::response::Value{startArg};
    end_ = gql::response::Value{endArg};
  }
  std::optional<gql::response::Value> getStart() const noexcept {
    return start_;
  }
  std::optional<gql::response::Value> getEnd() const noexcept { return end_; }
  std::optional<gql::response::Value> start_;
  std::optional<gql::response::Value> end_;
};
struct pattern {
  explicit pattern(
      gql::response::IdType&& idArg,
      const std::shared_ptr<otpo::Route>&& routeArg,
      const std::string&& codeArg,
      const std::optional<std::vector<std::shared_ptr<otpo::Stop>>>&& stopsArg,
      const std::optional<std::vector<std::shared_ptr<otpo::Coordinates>>>&&
          geometryArg) {
    id_ = std::move(idArg);
    route_ = routeArg;
    code_ = codeArg;
    stops_ = stopsArg;
    geometry_ = geometryArg;
  }
  gql::response::IdType getId() const noexcept { return id_; };
  std::shared_ptr<otpo::Route> getRoute() const noexcept { return route_; };
  std::string getCode() const noexcept { return code_; };
  std::optional<std::vector<std::shared_ptr<otpo::Stop>>> getStops()
      const noexcept {
    return stops_;
  };
  std::optional<std::vector<std::shared_ptr<otpo::Coordinates>>> getGeometry()
      const noexcept {
    return geometry_;
  };

  gql::response::IdType id_;
  std::shared_ptr<otpo::Route> route_;
  std::string code_;
  std::optional<std::vector<std::shared_ptr<otpo::Stop>>> stops_;
  std::optional<std::vector<std::shared_ptr<otpo::Coordinates>>> geometry_;
};

struct stoptime {
  explicit stoptime(
      const std::shared_ptr<otpo::Stop>&& stopArg,
      const std::optional<otp::RealtimeState>&& realtimeStateArg,
      const std::optional<otp::PickupDropoffType>&& pickupTypeArg) {
    stop_ = stopArg;
    realtime_state = realtimeStateArg;
    pickup_type = pickupTypeArg;
  }
  std::shared_ptr<otpo::Stop> getStop() const noexcept { return stop_; };
  std::optional<otp::RealtimeState> getRealtimeState() const noexcept {
    return realtime_state;
  };
  std::optional<otp::PickupDropoffType> getPickupType() const noexcept {
    return pickup_type;
  };

  std::shared_ptr<otpo::Stop> stop_;
  std::optional<otp::RealtimeState> realtime_state;
  std::optional<otp::PickupDropoffType> pickup_type;
};

struct agency {
  explicit agency(gql::response::IdType&& idArg, const std::string&& gtfsIdArg,
                  const std::string&& nameArg, const std::string&& urlArg) {
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
  /* Phone number which customers can use to contact this agency */
  std::optional<std::string> phone_;
  /* URL to a web page which has information of fares used by this agency */
  std::optional<std::string> fare_url;
};

struct trip {
  explicit trip(
      gql::response::IdType&& idArg, const std::string&& gtfsIdArg,
      const std::optional<std::string>&& tripHeadsignArg,
      const std::optional<std::string>&& directionIdArg,
      const std::shared_ptr<otpo::Pattern>&& patternArg,
      const std::optional<std::vector<std::shared_ptr<otpo::Stoptime>>>&&
          stoptimesArg,
      const std::optional<std::vector<std::shared_ptr<otpo::Stoptime>>>&&
          stoptimesForDateArg) {
    id_ = std::move(idArg);
    gtfs_id = gtfsIdArg;
    trip_headsign = tripHeadsignArg;
    direction_id = directionIdArg;
    pattern_ = patternArg;
    stoptimes_ = stoptimesArg;
    stoptimes_for_date = stoptimesForDateArg;
  };

  gql::response::IdType getId() const noexcept { return id_; };
  std::string getGtfsId() const noexcept { return gtfs_id; };
  std::optional<std::string> getTripHeadsign(
      std::optional<std::string>&& languageArg) const noexcept {
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
  getStoptimesForDate(
      std::optional<std::string>&& serviceDateArg) const noexcept {
    return stoptimes_for_date;
  };

  gql::response::IdType id_;
  std::string gtfs_id;
  std::optional<std::string> trip_headsign;
  std::optional<std::string> direction_id;
  std::shared_ptr<otpo::Pattern> pattern_;
  std::optional<std::vector<std::shared_ptr<otpo::Stoptime>>> stoptimes_;
  /*
   * stoptimesForDate(
   * """Date for which stoptimes are returned. Format: YYYYMMDD"""
   * serviceDate: String
   * ): [Stoptime]
   */
  std::optional<std::vector<std::shared_ptr<otpo::Stoptime>>>
      stoptimes_for_date;
};

struct route {
  explicit route(
      gql::response::IdType&& idArg, const std::string&& gtfsIdArg,
      const std::shared_ptr<otpo::Agency>&& agencyArg,
      const std::optional<std::string>&& shortNameArg,
      const std::optional<std::string>&& longNameArg,
      const std::optional<otp::TransitMode>&& modeArg,
      const std::optional<int>&& typeArg,
      const std::optional<std::string>&& urlArg,
      const std::optional<std::string>&& colorArg,
      const std::optional<std::vector<std::shared_ptr<otpo::Alert>>>&&
          alertsArg) {
    id_ = std::move(idArg);
    gtfs_id = gtfsIdArg;
    agency_ = agencyArg;
    short_name = shortNameArg;
    long_name = longNameArg;
    mode_ = std::move(modeArg);
    type_ = typeArg;
    url_ = urlArg;
    color_ = colorArg;
    alerts_ = alertsArg;
  }

  gql::response::IdType getId() const noexcept { return id_; };
  std::string getGtfsId() const noexcept { return gtfs_id; };
  std::shared_ptr<otpo::Agency> getAgency() const noexcept { return agency_; };
  std::optional<std::string> getShortName() const noexcept {
    return short_name;
  };
  std::optional<std::string> getLongName(
      std::optional<std::string>&& languageArg) const noexcept {
    return long_name;
  };
  std::optional<otp::TransitMode> getMode() const noexcept { return mode_; };
  std::optional<int> getType() const noexcept { return type_; };
  std::optional<std::string> getDesc() const noexcept { return desc_; };
  std::optional<std::string> getUrl() const noexcept { return url_; };
  std::optional<std::string> getColor() const noexcept { return color_; };
  std::optional<std::vector<std::shared_ptr<otpo::Alert>>> getAlerts(
      std::optional<std::vector<std::optional<otp::RouteAlertType>>>&& typesArg)
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
  explicit stop(gql::response::IdType&& idArg, const std::string&& gtfsIdArg,
                const std::string&& nameArg,
                const std::optional<double>&& latArg,
                const std::optional<double>&& lonArg,
                const std::optional<otp::Mode>&& vehicleModeArg,
                const std::vector<std::shared_ptr<otpo::Alert>>&& alertsArg) {
    id_ = std::move(idArg);
    gtfs_id = gtfsIdArg;
    name_ = nameArg;
    lat_ = latArg;
    lon_ = lonArg;
    vehicle_mode_ = vehicleModeArg;
    alerts_ = alertsArg;
  }

  gql::response::IdType getId() const noexcept { return id_; };
  std::string getGtfsId() const noexcept { return gtfs_id; };
  std::string getName(std::optional<std::string>&& languageArg) const noexcept {
    return name_;
  };
  std::optional<double> getLat() const noexcept { return lat_; };
  std::optional<double> getLon() const noexcept { return lon_; };
  std::optional<std::string> getCode() const noexcept { return code_; };
  std::optional<std::string> getDesc(
      std::optional<std::string>&& languageArg) const noexcept {
    return desc_;
  };
  std::optional<std::string> getZoneId() const noexcept { return zone_id; };
  gql::service::AwaitableScalar<std::optional<otp::Mode>> getVehicleMode(
      gql::service::FieldParams&& params) const noexcept {
    return vehicle_mode_;
  }
  std::optional<std::string> getPlatformCode() const noexcept {
    return platform_code;
  };
  std::optional<std::vector<std::shared_ptr<otpo::Alert>>> getAlerts(
      std::optional<std::vector<std::optional<otp::StopAlertType>>>&& typesArg)
      const noexcept {
    return alerts_;
  };
  std::optional<std::vector<std::shared_ptr<otpo::Stop>>> getStops()
      const noexcept {
    return stops_;
  }

  gql::response::IdType id_;
  std::string gtfs_id;
  std::string name_;
  std::optional<double> lat_;
  std::optional<double> lon_;
  /* Stop code which is visible at the stop  */
  std::optional<std::string> code_;
  /* Description of the stop, usually a street name */
  std::optional<std::string> desc_;
  /* ID of the zone where this stop is located */
  std::optional<std::string> zone_id;
  std::optional<otp::Mode> vehicle_mode_;
  /* Identifier of the platform, usually a number. This value is only present
   * for stops that are part of a station */
  std::optional<std::string> platform_code;
  std::optional<std::vector<std::shared_ptr<otpo::Alert>>> alerts_;
  /*Returns all stops that are children of this station (Only applicable for
   * stations)*/
  std::optional<std::vector<std::shared_ptr<otpo::Stop>>> stops_;
};

struct place {
  explicit place(const double latArg, const double lonArg,
                 const std::optional<std::string>&& nameArg,
                 const otp::VertexType&& vertexTypeArg,
                 const int arrivalTimeArg,
                 const std::shared_ptr<otpo::Stop>&& stopArg) {
    lat_ = latArg;
    lon_ = lonArg;
    name_ = nameArg;
    vertex_type = vertexTypeArg;
    arrival_time = gql::response::Value{arrivalTimeArg};
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
  std::shared_ptr<otpo::CarPark> getCarPark() const noexcept {
    return car_park_;
  }
  std::shared_ptr<otpo::VehicleParking> getVehicleParking() const noexcept {
    return vehicle_parking;
  };
  double lat_;
  double lon_;
  gql::response::Value arrival_time;
  std::optional<std::string> name_;
  std::optional<otp::VertexType> vertex_type;
  std::shared_ptr<otpo::Stop> stop_;
  /* The bike rental station related to the place */
  std::shared_ptr<otpo::BikeRentalStation> bike_rental_station;
  /* The bike parking related to the place */
  std::shared_ptr<otpo::BikePark> bike_park;
  /* The car parking related to the place */
  std::shared_ptr<otpo::CarPark> car_park_;
  /* The vehicle parking related to the place */
  std::shared_ptr<otpo::VehicleParking> vehicle_parking;
};

struct geometry {
  explicit geometry(const int lengthArg, const std::string&& pointsArg) {
    length_ = lengthArg;
    points_ = gql::response::Value{gql::response::StringType{pointsArg}};
  };

  std::optional<int> getLength() const noexcept { return length_; };
  std::optional<gql::response::Value> getPoints() const noexcept {
    return points_;
  };
  std::optional<int> length_;
  std::optional<gql::response::Value> points_;
};

struct leg {
  explicit leg(const std::optional<otp::Mode>&& modeArg,
               const std::shared_ptr<otpo::Agency>&& agencyArg,
               const std::shared_ptr<otpo::Place>&& fromArg,
               const std::shared_ptr<otpo::Place>&& toArg,
               const std::shared_ptr<otpo::Geometry>&& legGeometryArg,
               const std::optional<std::vector<std::shared_ptr<otpo::Place>>>&&
                   intermediatePlacesArg,
               const std::optional<bool>&& realtimeArg,
               const std::optional<bool>&& transitLegArg, int startTimeArg,
               int endTimeArg, const std::optional<int>&& departureDelayArg,
               const std::optional<int>&& arrivalDelayArg,
               const std::optional<bool>&& interlineWithPreviousLegArg,
               const std::optional<double>&& distanceArg,
               const std::optional<double>&& durationArg,
               const std::optional<bool>&& intermediatePlaceArg,
               const std::shared_ptr<otpo::Route>&& routeArg,
               const std::shared_ptr<otpo::Trip>&& tripArg) {
    mode_ = modeArg;
    agency_ = agencyArg;
    from_ = fromArg;
    to_ = toArg;
    leg_geometry = legGeometryArg;
    intermediate_places = intermediatePlacesArg;
    realtime_ = realtimeArg;
    transit_leg = transitLegArg;
    start_time = gql::response::Value{startTimeArg};
    end_time = gql::response::Value{endTimeArg};
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
  std::optional<gql::response::Value> getEndTime() const noexcept {
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

  std::optional<otp::Mode> mode_;
  std::shared_ptr<otpo::Agency> agency_;
  std::shared_ptr<otpo::Place> from_;
  std::shared_ptr<otpo::Place> to_;
  /*
   * Special booking information for the drop off stop of this leg if, for
   * example, it needs to be booked in advance. This could be due to a flexible
   * or on-demand service.
   */
  std::shared_ptr<otpo::BookingInfo> drop_off_booking_info;
  std::shared_ptr<otpo::Geometry> leg_geometry;
  std::optional<std::vector<std::shared_ptr<otpo::Place>>> intermediate_places;
  std::optional<bool> realtime_;
  /* State of real-time data */
  std::optional<otp::RealtimeState> realtime_state;
  std::optional<bool> transit_leg;
  /* Whether this leg is traversed with a rented bike. */
  std::optional<bool> rented_bike;
  std::optional<gql::response::Value> start_time;
  std::optional<gql::response::Value> end_time;
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
  explicit itinerary(
      const int startTimeArg, const int endTimeArg, const int durationArg,
      const double walkDistArg,
      const std::vector<std::shared_ptr<otpo::Leg>>&& legsArg,
      const std::vector<std::shared_ptr<otpo::fare>>&& faresArg) {
    start_time = gql::response::Value{startTimeArg};
    end_time = gql::response::Value{endTimeArg};
    duration_ = gql::response::Value{durationArg};
    walk_distance = walkDistArg;
    legs_ = std::move(legsArg);
    fares_ = std::move(faresArg);
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
  std::optional<bool> getArrivedAtDestinationWithRentedBicycle()
      const noexcept {
    return arrived_at_dest_with_rented_bicycle;
  }

  std::optional<gql::response::Value> start_time;
  std::optional<gql::response::Value> end_time;
  std::optional<gql::response::Value> duration_;
  std::optional<double> walk_distance;
  /* Does the itinerary end without dropping off the rented bicycle */
  std::optional<bool> arrived_at_dest_with_rented_bicycle;
  std::vector<std::shared_ptr<otpo::Leg>> legs_;
  /* Information about the fares for this itinerary. This is primarily a GTFS
   * Fares V1 interface will be removed in the future.
   */
  std::optional<std::vector<std::shared_ptr<otpo::fare>>> fares_;
};

/////////////////////////////////////// Begin
//////////////////////////////////////////

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

/*
 * Create object of struct trip using an object of type journey:transport and journey:stop.
 * @param [feedId] id of the area in which this traffic service is operating.
 */
std::shared_ptr<otpo::Place> CreatePlaceWithTransport(
    const journey::stop& stopArg, const journey::transport& transport,
    const std::string& feedId) {
  const double lat = stopArg.lat_;
  const double lon = stopArg.lng_;

  const std::string stopName = stopArg.name_;

  auto const arrivalTime = stopArg.arrival_.timestamp_;

  otp::VertexType vertexType;
  std::shared_ptr<otpo::Stop> stop_place = nullptr;
  if (stopName == "START" || stopName == "END") {
    vertexType = otp::VertexType::NORMAL;
  } else {
    vertexType = otp::VertexType::TRANSIT;

    ////// Create stop
    auto stopID =
        gql::response::IdType{gql::response::StringType{stopArg.eva_no_}};

    const std::string gtfsId = feedId + ":" + stopArg.eva_no_;
    auto const vehicleMode = getModeFromStation(transport);

    auto const alerts = std::vector<std::shared_ptr<otpo::Alert>>{};

    stop_place = std::make_shared<otpo::Stop>(std::make_shared<stop>(
        std::move(stopID), std::move(gtfsId), std::move(stopName), lat, lon,
        std::move(vehicleMode), std::move(alerts)));
    ////// End stop
  }
  return std::make_shared<otpo::Place>(
      std::make_shared<place>(lat, lon, stopName, std::move(vertexType),
                              arrivalTime, std::move(stop_place)));
}

/*
 * Create object of struct trip using an object of type journey:transport and journey:trip.
 * @param [agency] agency that provided this traffic service.
 * @param [alerts] alerts about this trip.
 * @param [feedId] id of the area in which this traffic service is operating.
 */
std::shared_ptr<otpo::Trip> CreateTripWithTransport(
    const std::vector<std::shared_ptr<otpo::Alert>>& alertsArg,
    const std::shared_ptr<otpo::Route>& routeArg,
    const journey::transport& transport, const journey::trip& tripJourneyArg,
    const std::string& feedId) {

  mm::message_creator fbb;
  fbb.create_and_finish(
      MsgContent_TripId,
      CreateTripId(
          fbb, fbb.CreateString(tripJourneyArg.extern_trip_.id_),
          fbb.CreateString(tripJourneyArg.extern_trip_.station_id_),
          tripJourneyArg.extern_trip_.train_nr_,
          tripJourneyArg.extern_trip_.time_,
          fbb.CreateString(tripJourneyArg.extern_trip_.target_station_id_),
          tripJourneyArg.extern_trip_.target_time_,
          fbb.CreateString(tripJourneyArg.extern_trip_.line_id_))
          .Union(),
      "/trip_to_connection");

  auto const res = motis_call(make_msg(fbb));
  auto const con = motis_content(Connection, res->val());
  auto journey = motis::convert(con);

  auto id_trip = gql::response::IdType(
      gql::response::StringType{tripJourneyArg.extern_trip_.id_});
  const std::string gtfs_id_trip = "unknown_trip_gtfs_id";
  const std::string trip_headsign = transport.direction_;

  //???? how to determine if the trip is an offbound or inbound travel
  std::optional<std::string> direction_id_trip = "0";

  /////// Create pattern
  gql::response::IdType id_pattern =
      gql::response::IdType{gql::response::StringType{"unknown_pattern_id"}};
  auto const route_pattern = routeArg;
  const std::string code_pattern = "unknown_pattern_code";

  auto stops_pattern = std::vector<std::shared_ptr<otpo::Stop>>{};
  for (auto& stop_iter : journey.stops_) {
    auto stopID =
        gql::response::IdType{gql::response::StringType{stop_iter.eva_no_}};
    auto const stopName = stop_iter.name_;
    auto const lat = stop_iter.lat_;
    auto const lon = stop_iter.lng_;
    // gtfsId
    const std::string gtfsId = feedId + ":" + stop_iter.eva_no_;
    otp::Mode mode;
    auto alerts = alertsArg;
    stops_pattern.push_back(std::make_shared<otpo::Stop>(std::make_shared<stop>(
        std::move(stopID), std::move(gtfsId), std::move(stopName), lat, lon,
        std::move(mode), std::move(alerts))));
  }

  // unknown coordinates not relevant with stop above
  auto const geometry_pattern =
      std::vector<std::shared_ptr<otpo::Coordinates>>{};

  auto const stops_stoptime = stops_pattern;

  auto const pattern_trip =
      std::make_shared<otpo::Pattern>(std::make_shared<pattern>(
          std::move(id_pattern), std::move(route_pattern),
          std::move(code_pattern), std::move(stops_pattern),
          std::move(geometry_pattern)));
  /////// End pattern

  /////// Start Stoptime
  auto stoptimes_trip = std::vector<std::shared_ptr<otpo::Stoptime>>{};

  for (auto& stp : stops_pattern) {
    std::shared_ptr<otpo::Stop> stop_ST = stp;

    // set as scheduled
    auto const realtime_state_ST = otp::RealtimeState::SCHEDULED;
    // set as scheduled
    auto const pickup_type_ST = otp::PickupDropoffType::SCHEDULED;

    stoptimes_trip.push_back(
        std::make_shared<otpo::Stoptime>(std::make_shared<stoptime>(
            std::move(stop_ST), std::move(realtime_state_ST),
            std::move(pickup_type_ST))));
  }
  /////// End Stoptime

  ////// the parameter is the date understandable but the type stoptime
  /// that was returned doesn't make no sense at all
  auto stoptimes_for_date_trip = std::vector<std::shared_ptr<otpo::Stoptime>>{};

  return std::make_shared<otpo::Trip>(std::make_shared<trip>(
      std::move(id_trip), std::move(gtfs_id_trip), trip_headsign,
      std::move(direction_id_trip), std::move(pattern_trip),
      std::move(stoptimes_trip), std::move(stoptimes_for_date_trip)));
}

/*
 * Create object of struct route using an object of type journey:transport.
 * @param [agency] agency that provided this traffic service.
 * @param [alerts] alerts about this route.
 * @param [feedId] id of the area in which this traffic service is operating.
 */
std::shared_ptr<otpo::Route> CreateRouteWithTransport(
    const std::shared_ptr<otpo::Agency>& agency,
    const std::vector<std::shared_ptr<otpo::Alert>>& alerts,
    const journey::transport& transport, const std::string& feedId) {

  std::string color = "FF0000";  // don't have info
  std::string gtfsId = feedId + ":" + "unknown_route_gtfsid";
  auto id = gql::response::IdType{
      gql::response::StringType{"unknown_route_id"}};  // don't have info
  std::string longName = transport.name_;
  std::string shortName = transport.name_;

  otp::TransitMode mode;
  int type;
  switch (transport.clasz_) {
    case 0: {
      type = -1;
      mode = otp::TransitMode::AIRPLANE;
      break;
    }
    case 1:
    case 2:
    case 4:
    case 5:
    case 6:
    case 7:
      type = 2;
      mode = otp::TransitMode::RAIL;
      break;
    case 3:
    case 10:
      type = 3;
      mode = otp::TransitMode::BUS;
      break;
    case 8:
      type = 1;
      mode = otp::TransitMode::SUBWAY;
      break;
    case 9:
      type = 0;
      mode = otp::TransitMode::TRAM;
      break;
    case 11:
      type = 4;
      mode = otp::TransitMode::FERRY;
      break;
  }
  std::string url = "";  // don't have info
  return std::make_shared<otpo::Route>(std::make_shared<route>(
      std::move(id), std::move(gtfsId), std::move(agency), shortName, longName,
      mode, type, url, color, std::move(alerts)));
  ;
}

/*
 * Caculates the distance using coordinates of begin and end point
 * with the desired means of transportation.
 */
const std::shared_ptr<motis::module::message> caculateDistanceViaOSRM(
    double start_lat, double start_lon, double dest_lat, double dest_lon,
    std::string const profile) {
  auto const waypoints =
      std::vector<Position>{{start_lat, start_lon}, {dest_lat, dest_lon}};

  mm::message_creator mc;
  mc.create_and_finish(
      MsgContent_OSRMViaRouteRequest,
      osrm::CreateOSRMViaRouteRequest(mc, mc.CreateString(profile),
                                      mc.CreateVectorOfStructs(waypoints))
          .Union(),
      "/osrm/via");

  auto const response = motis_call(make_msg(mc));
  auto const osrm_msg = response->val();

  return osrm_msg;
}

const journey::trip& GetJourneyTripAccordingToTransport(
    const journey::transport& transport,
    const std::vector<journey::trip>& jn_trips) {
  for (auto& jn_trip : jn_trips) {
    if (jn_trip.from_ == transport.from_ && jn_trip.to_ == transport.to_) {
      return jn_trip;
    }
  }
  throw std::runtime_error("jounery trip error");
}

geo::polyline convertCoordVectorToPolyline(
    const flatbuffers::Vector<double>* coords) {
  double lat_ = 0;
  double lon_ = 0;

  geo::polyline polyline;
  for (size_t i = 0; i < coords->size(); i++) {
    if (i % 2 == 0) {
      lat_ = coords->Get(i);
    } else {
      lon_ = coords->Get(i);
      polyline.push_back({lat_, lon_});
    }
  }
  return polyline;
}

std::string getFeedID(const std::string& idArg) {
  std::stringstream ss(idArg);
  std::string str;

  getline(ss, str, '_');

  return str;
}

/*
 * Checks whether the transport mode returned in the motis response
 * is within the modes expected by the user.
 */
bool checkModeFromConnection(
    const journey::transport& tran,
    const std::vector<std::unique_ptr<otp::TransportMode>>& transportModes) {
  auto const mode = getModeFromStation(tran);
  for (auto transIter = transportModes.begin();
       transIter != transportModes.end(); transIter++) {
    if (mode == (*transIter)->mode) {
      return true;
    }
  }
  return false;
}

/*
 * Create object of struct itinerary based on object of type connection.
 */
std::shared_ptr<otpo::Itinerary> createItineraryViaConnection(
    const Connection* con,
    std::vector<std::unique_ptr<otp::TransportMode>>& transportModes) {
  auto const journey = motis::convert(con);

  if (transportModes.size() == 6) {
    auto tryy = 1;
    tryy++;
  }
  for (auto const trans : journey.transports_) {
    if (!checkModeFromConnection(trans, transportModes)) {
      return nullptr;
    }
  }

  auto const start_time_Itinerary =
      journey.stops_.begin()->departure_.timestamp_;
  auto const end_time_Itinerary =
      (journey.stops_.rbegin())->arrival_.timestamp_;
  auto const duration_Itinerary = end_time_Itinerary - start_time_Itinerary;
  auto walk_distance_Itinerary = 0;
  auto const fares = std::vector<std::shared_ptr<otpo::fare>>{};

  /////// create legs_Itinerary
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

    // Get FeedID
    auto const feedId = getFeedID(stop_from.eva_no_);

    // create osrm for geometry and distance
    double distance_ = 0;
    geo::polyline polyline_leggeo;

    if (tran.is_walk_) {
      auto const osrm_msg = caculateDistanceViaOSRM(
          stop_from.lat_, stop_from.lng_, stop_to.lat_, stop_to.lng_, "foot");
      auto const osrm_res = motis_content(OSRMViaRouteResponse, osrm_msg);
      distance_ = osrm_res->distance();
      polyline_leggeo =
          convertCoordVectorToPolyline(osrm_res->polyline()->coordinates());
    } else if (mode == otp::Mode::BUS) {
      auto const osrm_msg = caculateDistanceViaOSRM(
          stop_from.lat_, stop_from.lng_, stop_to.lat_, stop_to.lng_, "bus");
      auto const osrm_res = motis_content(OSRMViaRouteResponse, osrm_msg);
      distance_ = osrm_res->distance();
      polyline_leggeo =
          convertCoordVectorToPolyline(osrm_res->polyline()->coordinates());
    } else {
      distance_ = geo::distance(geo::latlng{stop_from.lat_, stop_from.lng_},
                                geo::latlng{stop_to.lat_, stop_to.lng_});
      polyline_leggeo = {{stop_from.lat_, stop_from.lng_},
                         {stop_to.lat_, stop_to.lng_}};
    }

    if (tran.is_walk_) {
      walk_distance_Itinerary += distance_;
    }

    /////// Create Leggeometry
    auto length_leggeo = polyline_leggeo.size();
    auto const polyline_leggeo_encoded = geo::encode_polyline(polyline_leggeo);
    auto const leggeo =
        std::make_shared<otpo::Geometry>(std::make_shared<geometry>(
            length_leggeo, std::move(polyline_leggeo_encoded)));
    /////// End Leggeometry

    /////// Create Agency
    std::shared_ptr<otpo::Agency> agc_ = nullptr;
    if (!tran.is_walk_) {
      auto agency_id =
          gql::response::IdType(gql::response::StringType{"unknown_agency_id"});
      std::string agency_gtfs_id = feedId + ':' + "unknown_agency_id";
      std::string agency_name = tran.provider_;
      std::string agency_url = "unknown_agency_url";
      agc_ = std::make_shared<otpo::Agency>(std::make_shared<agency>(
          std::move(agency_id), std::move(agency_gtfs_id),
          std::move(agency_name), std::move(agency_url)));
    }

    auto const real_time = false;  // immmer false ??

    bool transit_leg = true;
    if (stop_from.name_ == "START" || stop_to.name_ == "END") {
      transit_leg = false;
    }

    auto const alerts =
        std::vector<std::shared_ptr<otpo::Alert>>{};  // immer leer ??

    auto const from =
        CreatePlaceWithTransport(journey.stops_.at(tran.from_), tran, feedId);

    auto const to =
        CreatePlaceWithTransport(journey.stops_.at(tran.to_), tran, feedId);

    std::shared_ptr<otpo::Route> route_ = nullptr;
    std::shared_ptr<otpo::Trip> trip_ = nullptr;
    if (!tran.is_walk_) {
      // route
      route_ = CreateRouteWithTransport(agc_, alerts, tran, feedId);
      // trip
      auto jn_trip = GetJourneyTripAccordingToTransport(tran, journey.trips_);
      trip_ = CreateTripWithTransport(alerts, route_, tran, jn_trip, feedId);
    }

    std::vector<std::shared_ptr<otpo::Place>> intermediatePlaces;
    if (!tran.is_walk_) {
      for (size_t count = tran.from_ + 1; count < tran.to_; count++) {
        intermediatePlaces.push_back(
            CreatePlaceWithTransport(journey.stops_.at(count), tran, feedId));
      }
    }

    bool intermediatePlace = false;  // immmer false ??
    bool interlineWithPreviousLeg = false;  // immmer false ??

    auto const l = std::make_shared<leg>(
        mode, std::move(agc_), std::move(from), std::move(to),
        std::move(leggeo), std::move(intermediatePlaces), real_time,
        transit_leg, start_time_leg, end_time_leg, departure_delay,
        arrival_delay, interlineWithPreviousLeg, distance_, duration_leg,
        intermediatePlace, std::move(route_), std::move(trip_));
    auto const otpLeg = std::make_shared<otpo::Leg>(l);

    legs.push_back(otpLeg);
  }

  return std::make_shared<otpo::Itinerary>(std::make_shared<itinerary>(
      start_time_Itinerary, end_time_Itinerary, duration_Itinerary,
      walk_distance_Itinerary, std::move(legs), std::move(fares)));
}

/*
 * Create object of struct itinerary based on object of type route.
 * This itinerary object is intended for pedestrians.
 */
std::shared_ptr<otpo::Itinerary> createItineraryViaRoute(const ppr::Route* r,
                                                         const int begin,
                                                         const otp::Mode mode) {

  /////// create other infos
  auto const start_time_Itinerary = begin;
  auto const end_time_Itinerary = begin + r->duration() * 60;
  auto const duration_Itinerary = r->duration();
  auto walk_distance_Itinerary = r->distance();
  auto const fares = std::vector<std::shared_ptr<otpo::fare>>{};

  /////// create legs_Itinerary
  std::vector<std::shared_ptr<otpo::Leg>> legs;

  auto const stop_from = r->start();
  auto const stop_to = r->destination();

  auto const start_time_leg = start_time_Itinerary;
  auto const end_time_leg = end_time_Itinerary;
  auto const departure_delay = 0;
  auto const arrival_delay = 0;
  auto const duration_leg = duration_Itinerary;

  // create osrm for geometry and distance
  double distance_ = 0;
  geo::polyline polyline_leggeo;

  auto const osrm_msg =
      caculateDistanceViaOSRM(stop_from->lat(), stop_from->lng(),
                              stop_to->lat(), stop_to->lng(), "foot");
  auto const osrm_res = motis_content(OSRMViaRouteResponse, osrm_msg);
  distance_ = osrm_res->distance();
  polyline_leggeo =
      convertCoordVectorToPolyline(osrm_res->polyline()->coordinates());

  /////// Create Leggeometry
  auto length_leggeo = polyline_leggeo.size();
  auto const polyline_leggeo_encoded = geo::encode_polyline(polyline_leggeo);
  auto const leggeo =
      std::make_shared<otpo::Geometry>(std::make_shared<geometry>(
          length_leggeo, std::move(polyline_leggeo_encoded)));
  /////// End Leggeometry

  std::shared_ptr<otpo::Agency> agc_ = nullptr;
  auto const real_time = false;
  bool transit_leg = false;

  auto const from = std::make_shared<otpo::Place>(
      std::make_shared<place>(stop_from->lat(), stop_from->lng(), "",
                              otp::VertexType::NORMAL, 0, nullptr));

  auto const to = std::make_shared<otpo::Place>(std::make_shared<place>(
      stop_to->lat(), stop_to->lng(), "", otp::VertexType::NORMAL, 0, nullptr));

  std::shared_ptr<otpo::Route> route_ = nullptr;
  std::shared_ptr<otpo::Trip> trip_ = nullptr;
  std::vector<std::shared_ptr<otpo::Place>> intermediatePlaces{};

  bool intermediatePlace = false;
  bool interlineWithPreviousLeg = false;

  auto const l = std::make_shared<leg>(
      mode, std::move(agc_), std::move(from), std::move(to), std::move(leggeo),
      std::move(intermediatePlaces), real_time, transit_leg, start_time_leg,
      end_time_leg, departure_delay, arrival_delay, interlineWithPreviousLeg,
      distance_, duration_leg, intermediatePlace, std::move(route_),
      std::move(trip_));
  auto const otpLeg = std::make_shared<otpo::Leg>(l);

  legs.push_back(otpLeg);

  return std::make_shared<otpo::Itinerary>(std::make_shared<itinerary>(
      start_time_Itinerary, end_time_Itinerary, duration_Itinerary,
      walk_distance_Itinerary, std::move(legs), std::move(fares)));
}

std::tuple<std::string, double, double> convertStringToCoordinate(
    const std::string& arg) {
  std::stringstream ss(arg);
  std::string str;

  getline(ss, str, ':');
  auto const address_ = str;
  getline(ss, str, ':');

  getline(ss, str, ',');
  auto const lat_ = std::stod(str);
  getline(ss, str, ',');
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
  auto const start_unix_time =
      motis::parse_unix_time(date_str, "%Y-%m-%d %H:%M:%S %Z");

  return start_unix_time;
};

std::shared_ptr<mm::message> createIntermodalRouting(
    const int& begin, const int& end,
    const std::tuple<std::string, double, double>& fromPos,
    const std::tuple<std::string, double, double>& toPos,
    std::optional<int>& numItinerariesArg) {
  mm::message_creator mc;

  auto const interval = Interval(begin, end);

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

  return res_value;
}

std::shared_ptr<mm::message> createPPRRouting(
    const std::tuple<std::string, double, double>& fromPos,
    const std::tuple<std::string, double, double>& toPos) {
  const Position start_pos{std::get<1>(fromPos), std::get<2>(fromPos)};

  mm::message_creator mc;

  mc.create_and_finish(
      MsgContent_FootRoutingRequest,
      CreateFootRoutingRequest(
          mc, &start_pos,
          mc.CreateVectorOfStructs(std::vector<Position>{
              Position(std::get<1>(toPos), std::get<2>(toPos))}),
          ppr::CreateSearchOptions(mc, mc.CreateString("default"), 10000),
          SearchDir::SearchDir_Forward, true, true, true)
          .Union(),
      "/ppr/route");

  auto const response = motis_call(make_msg(mc));
  auto const res_val = response->val();
  return res_val;
}

struct plan {
  explicit plan(
      std::optional<std::string>&& dateArg,
      std::optional<std::string>&& timeArg,
      std::optional<std::string>&& fromPlaceArg,
      std::optional<std::string>&& toPlaceArg,
      std::optional<int>&& numItinerariesArg,
      std::optional<std::vector<std::unique_ptr<otp::TransportMode>>>&&
          transportModesArg) {

    // Create Intermodal Routing Request
    mm::message_creator mc;
    auto const begin = convertTime(dateArg.value(), timeArg.value());
    auto const end = convertTime(dateArg.value(), timeArg.value(), true);

    auto const fromPos = convertStringToCoordinate(fromPlaceArg.value());
    auto const toPos = convertStringToCoordinate(toPlaceArg.value());

    if ((transportModesArg.value().size() == 1) &&
        (*transportModesArg.value().begin())->mode == otp::Mode::WALK) {
      // Caculate walking route.
      auto ppr_msg = createPPRRouting(fromPos, toPos);
      auto const ppr_res = motis_content(FootRoutingResponse, ppr_msg);
      for (auto r : *ppr_res->routes()->begin()->routes()) {
        itineraries_.push_back(
            createItineraryViaRoute(r, begin, otp::Mode::WALK));
      }
    } else if ((transportModesArg.value().size() == 1) &&
               (*transportModesArg.value().begin())->mode ==
                   otp::Mode::BICYCLE) {
      // Calculate cycling route.
      auto ppr_msg = createPPRRouting(fromPos, toPos);
      auto const ppr_res = motis_content(FootRoutingResponse, ppr_msg);
      auto re = *ppr_res->routes()->begin()->routes();
      for (auto r : *ppr_res->routes()->begin()->routes()) {
        itineraries_.push_back(
            createItineraryViaRoute(r, begin, otp::Mode::BICYCLE));
      }
    } else {
      // itinerary init via intermodal routing
      auto intermodal_msg = createIntermodalRouting(begin, end, fromPos, toPos,
                                                    numItinerariesArg);

      //    std::cout <<
      //    res_value->to_json(mm::json_format::DEFAULT_FLATBUFFERS)
      //              << "\n";
      auto const intermodal_res =
          motis_content(RoutingResponse, intermodal_msg);
      for (auto c : *intermodal_res->connections()) {
        auto const itinerary =
            createItineraryViaConnection(c, transportModesArg.value());

        if (itinerary != nullptr) {
          itineraries_.push_back(itinerary);
        }
      }
    }

    date_ = gql::response::Value{begin};
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
      std::optional<std::vector<std::unique_ptr<otp::TransportMode>>>&&
          transportModesArg,
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
        std::move(toPlaceArg), std::move(numItinerariesArg),
        std::move(transportModesArg)));
  }

  std::shared_ptr<otpo::QueryType> getViewer() const noexcept {
    return std::make_shared<otpo::QueryType>(std::make_shared<Query>());
  };
  std::shared_ptr<otpo::serviceTimeRange> getServiceTimeRange() const noexcept {
    return std::make_shared<otpo::serviceTimeRange>(
        std::make_shared<service_time_range>(1, 2));
  }
};

graphql::graphql() : module("GraphQL", "graphql") {}

void graphql::init(motis::module::registry& reg) {
  reg.register_op(
      "/index/graphql",
      [](mm::msg_ptr const& msg) {
        auto const req = motis_content(HTTPRequest, msg);

        std::shared_ptr<gql::service::Request> service =
            std::make_shared<otp::Operations>(std::make_shared<Query>());

        auto payload = gql::response::parseJSON(req->content()->str());
        auto variablesItr = payload.find("variables"sv);
        //        std::cout << "Content:" << req->content()->str() <<
        //        std::endl;
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
        auto queryId = payload.find("id"sv);
        if (queryId == payload.end() ||
            queryId->second.type() != gql::response::Type::String) {
          throw std::runtime_error("Id missing");
        }
        auto id_ = queryId->second.get<gql::response::StringType>();

        auto query = gql::peg::parseString(
            queryItr->second.get<gql::response::StringType>());
        auto const response = gql::response::toJSON(
            service
                ->resolve({.query = query,
                           .operationName = id_,
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
