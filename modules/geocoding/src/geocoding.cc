#include "motis/geocoding/geocoding.h"

#include "motis/module/context/motis_call.h"
#include "motis/module/message.h"
#include "geos/vend/include_nlohmann_json.hpp"

using namespace motis::guesser;

namespace nh = geos_nlohmann;
namespace mm = motis::module;
namespace fbb = flatbuffers;

namespace motis::geocoding {
bool changeAtr = false;
geocoding::geocoding() : module("Geocoding", "geocoding") {}

struct geometry_geocoding {

  void from_json(const nh::json& j, geometry_geocoding& val) {
    j.at("geometry").get_to(val.coordinates_);
    j.at("type").get_to(val.type_);
  }

  void to_json(nh::json& j, geometry_geocoding& val) {
    j["geometry"] = val.coordinates_;
    j["type"] = val.type_;
  }

  std::tuple<double, double> coordinates_;
  std::string type_;
};

struct properties_geocoding_osm {
  /*
    "osm_id": 188238154,
   "extent": [
        8.8566132,
        48.5978695,
        8.8567463,
        48.5977943
    ],
    "country": "Deutschland",
    "city": "Herrenberg",
    "countrycode": "DE",
    "county": "Landkreis Böblingen",
    "type": "house",
    "osm_type": "W",
    "osm_key": "building",
    "housenumber": "1",
    "street": "Belchenstraße",
    "district": "Affstätt",
    "osm_value": "yes",
    "region": "Baden-Württemberg",
    "postalcode": "71083",
    "locality": "Herrenberg",
    "label": "Belchenstraße 1, 71083 Herrenberg",
    "name": "Belchenstraße 1, 71083 Herrenberg",
    "layer": "venue",
    "confidence": 1,
    "source": "openstreetmap"*/
};

struct properties_geocoding_digi {
  explicit properties_geocoding_digi(
      const std::string& idArg, const std::string& gidArg,
      const std::string& layerArg, const std::string& sourceArg,
      const std::string& source_idArg, const std::string& nameArg,
      const std::string& postalcodeArg, const std::string& postalcode_gidArg,
      const int& confidenceArg, const int& distanceArg,
      const std::string& accuracyArg, const std::string& countryArg,
      const std::string& country_gidArg, const std::string& country_aArg,
      const std::string& regionArg, const std::string& region_gidArg,
      const std::string& localadminArg, const std::string& localadmin_gidArg,
      const std::string& localityArg, const std::string& locality_gidArg,
      const std::string& neighbourhoodArg,
      const std::string& neighbourhood_gidArg, const std::string& labelArg,
      const std::vector<std::string>& zonesArg) {
    id_ = idArg;
    gid_ = gidArg;
    layer_ = layerArg;
    source_ = sourceArg;
    source_id_ = source_idArg;
    name_ = nameArg;
    postalcode_ = postalcodeArg;
    postalcode_gid_ = postalcode_gidArg;
    confidence_ = confidenceArg;
    distance_ = distanceArg;
    accuracy_ = accuracyArg;
    country_ = countryArg;
    country_gid_ = country_gidArg;
    country_a_ = country_aArg;
    region_ = regionArg;
    region_gid_ = region_gidArg;
    localadmin_ = localadminArg;
    localadmin_gid_ = localadmin_gidArg;
    locality_ = localityArg;
    locality_gid_ = locality_gidArg;
    neighbourhood_ = neighbourhoodArg;
    neighbourhood_gid_ = neighbourhood_gidArg;
    label_ = labelArg;
    zones_ = std::move(zonesArg);
  };

  void from_json(const nh::json& j, properties_geocoding_digi& val) {
    j.at("geometry").get_to(val.id_);
    j.at("type").get_to(val.gid_);
    j.at("properties").get_to(val.layer_);
  }

  void to_json(nh::json& j, properties_geocoding_digi& val) {
    j["geometry"] = val.id_;
    j["type"] = val.gid_;
    j["properties"] = val.layer_;
  }

  std::string id_;
  std::string gid_;
  std::string layer_;
  std::string source_;
  std::string source_id_;
  std::string name_;
  std::string postalcode_;
  std::string postalcode_gid_;
  int confidence_;
  int distance_;
  std::string accuracy_;
  std::string country_;
  std::string country_gid_;
  std::string country_a_;
  std::string region_;
  std::string region_gid_;
  std::string localadmin_;
  std::string localadmin_gid_;
  std::string locality_;
  std::string locality_gid_;
  std::string neighbourhood_;
  std::string neighbourhood_gid_;
  std::string label_;
  std::vector<std::string> zones_;
};

struct features {

  void from_json(const nh::json& j, features& val) {
    val.geo_.from_json(j, val.geo_);
    j.at("type").get_to(val.type_);
    val.properties_.from_json(j, val.properties_);
  }

  void to_json(nh::json& j, features& val) {
    val.geo_.to_json(j, val.geo_);
    j["type"] = val.type_;
    val.properties_.to_json(j, val.properties_);
  }

  geometry_geocoding geo_;
  std::string type_;
  properties_geocoding_digi properties_;
};

void fillInfo(const StationGuesserResponse* station_res) {}

std::shared_ptr<mm::message> geocodingAddressViaPosition(
    const std::string& msg) {
  // must take variable out of the req
  const std::string point_lat = "48.60374409506935";
  const std::string point_lon = "8.879184722900392";
  auto radius = 0.1;
  auto lang = "de";
  auto size = 1;
  auto layers = "address";
  auto zones = 1;

  std::string input = point_lat + ";" + point_lon;
  mm::message_creator mc;
  mc.create_and_finish(
      MsgContent_StationGuesserRequest,
      CreateStationGuesserRequest(mc, size, mc.CreateString(input)).Union(),
      "/guesser");
  auto const response = motis_call(make_msg(mc));
  auto const res = response->val();
  return res;
}

void geocoding::init(motis::module::registry& reg) {
  reg.register_op(
      "/geocoding/reverse",
      [](mm::msg_ptr const& msg) {
        //        auto const req = motis_content(HTTPRequest, msg);

        //        auto response = req->content()->str();
        //        auto station_msg =
        //        geocodingAddressViaPosition(req->content()->str()); auto const
        //        station_res =
        //            motis_content(StationGuesserResponse, station_msg);

        auto from =
            "{\n"
            "\t\"features\": [\n"
            "\t\t{\n"
            "\t\t\t\"geometry\": {\n"
            "\t\t\t\t\"coordinates\": [\n"
            "\t\t\t\t\t6.105464,\n"
            "\t\t\t\t\t50.758075\n"
            "\t\t\t\t],\n"
            "\t\t\t\t\"type\": \"Point\"\n"
            "\t\t\t},\n"
            "\t\t\t\"type\": \"Feature\",\n"
            "\t\t\t\"properties\": {\n"
            "\t\t\t\t\"osm_id\": 25416882,\n"
            "\t\t\t\t\"extent\": [\n"
            "\t\t\t\t\t6.105464,\n"
            "\t\t\t\t\t50.758075\n"
            "\t\t\t\t],\n"
            "\t\t\t\t\"country\": \"Deutschland\",\n"
            "\t\t\t\t\"city\": \"Aachen\",\n"
            "\t\t\t\t\"countrycode\": \"DE\",\n"
            "\t\t\t\t\"county\": \"Landkreis Aachen\",\n"
            "\t\t\t\t\"type\": \"street\",\n"
            "\t\t\t\t\"osm_type\": \"W\",\n"
            "\t\t\t\t\"osm_key\": \"highway\",\n"
            "\t\t\t\t\"district\": \"Beverau\",\n"
            "\t\t\t\t\"osm_value\": \"track\",\n"
            "\t\t\t\t\"name\": \"Petronellastraße\",\n"
            "\t\t\t\t\"region\": \"Nordrhein-Westfalen\",\n"
            "\t\t\t\t\"postalcode\": \"52066\",\n"
            "\t\t\t\t\"locality\": \"Aachen\",\n"
            "\t\t\t\t\"label\": \"Petronellastraße 6, 52066 Aachen\",\n"
            "\t\t\t\t\"layer\": \"venue\",\n"
            "\t\t\t\t\"confidence\": 1,\n"
            "\t\t\t\t\"source\": \"openstreetmap\"\n"
            "\t\t\t}\n"
            "\t\t}\n"
            "\t]\n"
            "}";
        auto to =
            "{\n"
            "\t\"features\": [\n"
            "\t\t{\n"
            "\t\t\t\"geometry\": {\n"
            "\t\t\t\t\"coordinates\": [\n"
            "\t\t\t\t\t6.07519,\n"
            "\t\t\t\t\t50.782213999999996\n"
            "\t\t\t\t],\n"
            "\t\t\t\t\"type\": \"Point\"\n"
            "\t\t\t},\n"
            "\t\t\t\"type\": \"Feature\",\n"
            "\t\t\t\"properties\": {\n"
            "\t\t\t\t\"osm_id\": 25416882,\n"
            "\t\t\t\t\"extent\": [\n"
            "\t\t\t\t\t6.07519,\n"
            "\t\t\t\t\t50.782213999999996\n"
            "\t\t\t\t],\n"
            "\t\t\t\t\"country\": \"Deutschland\",\n"
            "\t\t\t\t\"city\": \"Aachen\",\n"
            "\t\t\t\t\"countrycode\": \"DE\",\n"
            "\t\t\t\t\"county\": \"Landkreis Aachen\",\n"
            "\t\t\t\t\"type\": \"street\",\n"
            "\t\t\t\t\"osm_type\": \"W\",\n"
            "\t\t\t\t\"osm_key\": \"highway\",\n"
            "\t\t\t\t\"district\": \"Mitte Aachen\",\n"
            "\t\t\t\t\"osm_value\": \"track\",\n"
            "\t\t\t\t\"name\": \"Brüggemannstraße\",\n"
            "\t\t\t\t\"region\": \"Nordrhein-Westfalen\",\n"
            "\t\t\t\t\"postalcode\": \"52072\",\n"
            "\t\t\t\t\"locality\": \"Aachen\",\n"
            "\t\t\t\t\"label\": \"Brüggemannstraße 4, 52072 Aachen\",\n"
            "\t\t\t\t\"layer\": \"venue\",\n"
            "\t\t\t\t\"confidence\": 1,\n"
            "\t\t\t\t\"source\": \"openstreetmap\"\n"
            "\t\t\t}\n"
            "\t\t}\n"
            "\t]\n"
            "}";
        std::string MsgSend = from;
        if (changeAtr) {
          MsgSend = to;
          changeAtr = !changeAtr;
        } else {
          MsgSend = from;
          changeAtr = !changeAtr;
        }
        mm::message_creator mc;
        mc.create_and_finish(
            MsgContent_HTTPResponse,
            CreateHTTPResponse(
                mc, HTTPStatus_OK,
                mc.CreateVector(std::vector<fbb::Offset<HTTPHeader>>{}),
                mc.CreateString(MsgSend))
                .Union());
        return make_msg(mc);
      },
      {});
  reg.register_op(
      "/geocoding/search",
      [](mm::msg_ptr const& msg) {
        //        auto const req = motis_content(HTTPRequest, msg);

        //        auto response = req->content()->str();
        //        auto station_msg =
        //        geocodingAddressViaPosition(req->content()->str()); auto const
        //        station_res =
        //            motis_content(StationGuesserResponse, station_msg);

        auto from =
            "{\n"
            "\t\"features\": [\n"
            "\t\t{\n"
            "\t\t\t\"geometry\": {\n"
            "\t\t\t\t\"coordinates\": [\n"
            "\t\t\t\t\t6.105464,\n"
            "\t\t\t\t\t50.758075\n"
            "\t\t\t\t],\n"
            "\t\t\t\t\"type\": \"Point\"\n"
            "\t\t\t},\n"
            "\t\t\t\"type\": \"Feature\",\n"
            "\t\t\t\"properties\": {\n"
            "\t\t\t\t\"osm_id\": 25416882,\n"
            "\t\t\t\t\"extent\": [\n"
            "\t\t\t\t\t6.105464,\n"
            "\t\t\t\t\t50.758075\n"
            "\t\t\t\t],\n"
            "\t\t\t\t\"country\": \"Deutschland\",\n"
            "\t\t\t\t\"city\": \"Aachen\",\n"
            "\t\t\t\t\"countrycode\": \"DE\",\n"
            "\t\t\t\t\"county\": \"Landkreis Aachen\",\n"
            "\t\t\t\t\"type\": \"street\",\n"
            "\t\t\t\t\"osm_type\": \"W\",\n"
            "\t\t\t\t\"osm_key\": \"highway\",\n"
            "\t\t\t\t\"district\": \"Beverau\",\n"
            "\t\t\t\t\"osm_value\": \"track\",\n"
            "\t\t\t\t\"name\": \"Petronellastraße\",\n"
            "\t\t\t\t\"region\": \"Nordrhein-Westfalen\",\n"
            "\t\t\t\t\"postalcode\": \"52066\",\n"
            "\t\t\t\t\"locality\": \"Aachen\",\n"
            "\t\t\t\t\"label\": \"Petronellastraße 6, 52066 Aachen\",\n"
            "\t\t\t\t\"layer\": \"venue\",\n"
            "\t\t\t\t\"confidence\": 1,\n"
            "\t\t\t\t\"source\": \"openstreetmap\"\n"
            "\t\t\t}\n"
            "\t\t}\n"
            "\t]\n"
            "}";
        auto to =
            "{\n"
            "\t\"features\": [\n"
            "\t\t{\n"
            "\t\t\t\"geometry\": {\n"
            "\t\t\t\t\"coordinates\": [\n"
            "\t\t\t\t\t6.07519,\n"
            "\t\t\t\t\t50.782213999999996\n"
            "\t\t\t\t],\n"
            "\t\t\t\t\"type\": \"Point\"\n"
            "\t\t\t},\n"
            "\t\t\t\"type\": \"Feature\",\n"
            "\t\t\t\"properties\": {\n"
            "\t\t\t\t\"osm_id\": 25416882,\n"
            "\t\t\t\t\"extent\": [\n"
            "\t\t\t\t\t6.07519,\n"
            "\t\t\t\t\t50.782213999999996\n"
            "\t\t\t\t],\n"
            "\t\t\t\t\"country\": \"Deutschland\",\n"
            "\t\t\t\t\"city\": \"Aachen\",\n"
            "\t\t\t\t\"countrycode\": \"DE\",\n"
            "\t\t\t\t\"county\": \"Landkreis Aachen\",\n"
            "\t\t\t\t\"type\": \"street\",\n"
            "\t\t\t\t\"osm_type\": \"W\",\n"
            "\t\t\t\t\"osm_key\": \"highway\",\n"
            "\t\t\t\t\"district\": \"Mitte Aachen\",\n"
            "\t\t\t\t\"osm_value\": \"track\",\n"
            "\t\t\t\t\"name\": \"Brüggemannstraße\",\n"
            "\t\t\t\t\"region\": \"Nordrhein-Westfalen\",\n"
            "\t\t\t\t\"postalcode\": \"52072\",\n"
            "\t\t\t\t\"locality\": \"Aachen\",\n"
            "\t\t\t\t\"label\": \"Brüggemannstraße 4, 52072 Aachen\",\n"
            "\t\t\t\t\"layer\": \"venue\",\n"
            "\t\t\t\t\"confidence\": 1,\n"
            "\t\t\t\t\"source\": \"openstreetmap\"\n"
            "\t\t\t}\n"
            "\t\t}\n"
            "\t]\n"
            "}";
        std::string MsgSend = from;
        if (changeAtr) {
          MsgSend = to;
          changeAtr = !changeAtr;
        } else {
          MsgSend = from;
          changeAtr = !changeAtr;
        }
        mm::message_creator mc;
        mc.create_and_finish(
            MsgContent_HTTPResponse,
            CreateHTTPResponse(
                mc, HTTPStatus_OK,
                mc.CreateVector(std::vector<fbb::Offset<HTTPHeader>>{}),
                mc.CreateString(MsgSend))
                .Union());
        return make_msg(mc);
      },
      {});
}

}  // namespace motis::geocoding