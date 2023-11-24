#include "motis/geocoding/geocoding.h"

#include "motis/module/context/motis_call.h"
#include "motis/module/message.h"
#include "geos/vend/json.hpp"

using namespace motis::guesser;
using namespace motis::address;

namespace nh = nlohmann;
namespace mm = motis::module;
namespace fbb = flatbuffers;

namespace motis::geocoding {
bool changeAtr = false;
geocoding::geocoding() : module("Geocoding", "geocoding") {}

struct geometry_geocoding {
  explicit geometry_geocoding(const double&& latArg, const double&& lonArg,
                              const std::string&& typeArg) {
    coordinates_ = std::make_tuple(latArg, lonArg);
    type_ = typeArg;
  };

  void from_json(const nh::json& j, geometry_geocoding& val) {
    j.at("coordinates").get_to(val.coordinates_);
    j.at("type_a").get_to(val.type_);
  }

  void to_json(nh::json& j, geometry_geocoding& val) {
    j["coordinates"] = val.coordinates_;
    j["type"] = val.type_;
  }

  std::tuple<double, double> coordinates_;
  std::string type_;
};

struct properties_geocoding_osm {
  explicit properties_geocoding_osm(
      const std::string& osmIdArg, const std::string& extentArg,
      const std::string& countryArg, const std::string& cityArg,
      const std::string& countrycodeArg, const std::string& countyArg,
      const std::string& typeArg, const std::string& osm_typeArg,
      const std::string& osm_keyArg, const std::string& housenumberArg,
      const std::string& streetArg, const std::string& districtArg,
      const std::string& osm_valueArg, const std::string& regionArg,
      const std::string& postalcodeArg, const std::string& localityArg,
      const std::string& labelArg, const std::string& nameArg,
      const std::string& layerArg, const int& confidenceArg,
      const std::string& sourceArg) {
    osm_id_ = osmIdArg;
    extent_ = extentArg;
    country_ = countryArg;
    city_ = cityArg;
    countrycode_ = countrycodeArg;
    county_ = countyArg;
    type_ = typeArg;
    osm_type_ = osm_typeArg;
    osm_key_ = osm_keyArg;
    housenumber_ = housenumberArg;
    street_ = streetArg;
    district_ = districtArg;
    osm_value_ = osm_valueArg;
    region_ = regionArg;
    postalcode_ = postalcodeArg;
    locality_ = localityArg;
    label_ = labelArg;
    name_ = nameArg;
    layer_ = layerArg;
    confidence_ = confidenceArg;
    source_ = sourceArg;
  };

  void from_json(const nh::json& j, properties_geocoding_osm& val) {
    j.at("osm_id").get_to(val.osm_id_);
    j.at("extent").get_to(val.extent_);
    j.at("country").get_to(val.country_);
    j.at("city").get_to(val.city_);
    j.at("countrycode").get_to(val.countrycode_);
    j.at("county").get_to(val.county_);
    j.at("type").get_to(val.type_);
    j.at("osm_type").get_to(val.osm_type_);
    j.at("osm_key").get_to(val.osm_key_);
    j.at("housenumber").get_to(val.housenumber_);
    j.at("street").get_to(val.street_);
    j.at("district").get_to(val.district_);
    j.at("osm_value").get_to(val.osm_value_);
    j.at("region").get_to(val.region_);
    j.at("postalcode").get_to(val.postalcode_);
    j.at("locality").get_to(val.locality_);
    j.at("label").get_to(val.label_);
    j.at("name").get_to(val.name_);
    j.at("layer").get_to(val.layer_);
    j.at("confidence").get_to(val.confidence_);
    j.at("source").get_to(val.source_);
  }

  void to_json(nh::json& j, properties_geocoding_osm& val) {
    j["osm_id"] = val.osm_id_;
    j["extent"] = val.extent_;
    j["country"] = val.country_;
    j["city"] = val.city_;
    j["countrycode"] = val.countrycode_;
    j["county"] = val.county_;
    j["type"] = val.type_;
    j["osm_type"] = val.osm_type_;
    j["osm_key"] = val.osm_key_;
    j["housenumber"] = val.housenumber_;
    j["street"] = val.street_;
    j["district"] = val.district_;
    j["osm_value"] = val.osm_value_;
    j["region"] = val.region_;
    j["postalcode"] = val.postalcode_;
    j["locality"] = val.locality_;
    j["label"] = val.label_;
    j["name"] = val.name_;
    j["layer"] = val.layer_;
    j["confidence"] = val.confidence_;
    j["source"] = val.source_;
  }

  std::string osm_id_;
  std::string extent_;
  std::string country_;
  std::string city_;
  std::string countrycode_;
  std::string county_;
  std::string type_;
  std::string osm_type_;
  std::string osm_key_;
  std::string housenumber_;
  std::string street_;
  std::string district_;
  std::string osm_value_;
  std::string region_;
  std::string postalcode_;
  std::string locality_;
  std::string label_;
  std::string name_;
  std::string layer_;
  int confidence_;
  std::string source_;
};

struct properties_geocoding_digi {
  explicit properties_geocoding_digi(const std::string& idArg,
                                     const std::string& gidArg,
                                     const std::string& layerArg) {
    id_ = idArg;
    gid_ = gidArg;
    layer_ = layerArg;
  };

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
    j.at("id").get_to(val.id_);
    j.at("gid").get_to(val.gid_);
    j.at("layer").get_to(val.layer_);
    j.at("source").get_to(val.source_);
    j.at("source_id").get_to(val.source_id_);
    j.at("name").get_to(val.name_);
    j.at("postalcode").get_to(val.postalcode_);
    j.at("postalcode_gid").get_to(val.postalcode_gid_);
    j.at("confidence").get_to(val.confidence_);
    j.at("distance").get_to(val.distance_);
    j.at("accuracy").get_to(val.accuracy_);
    j.at("country").get_to(val.country_);
    j.at("country_gid").get_to(val.country_gid_);
    j.at("country_a").get_to(val.country_a_);
    j.at("region").get_to(val.region_);
    j.at("region_gid").get_to(val.region_gid_);
    j.at("localadmin").get_to(val.localadmin_);
    j.at("localadmin_gid").get_to(val.localadmin_gid_);
    j.at("locality").get_to(val.locality_);
    j.at("locality_gid").get_to(val.locality_gid_);
    j.at("neighbourhood").get_to(val.neighbourhood_);
    j.at("neighbourhood_gid").get_to(val.neighbourhood_gid_);
    j.at("label").get_to(val.label_);
    j.at("zones_").get_to(val.zones_);
  }

  void to_json(nh::json& j, properties_geocoding_digi& val) {
    j["id"] = val.id_;
    j["gid"] = val.gid_;
    j["layer"] = val.layer_;
    j["source"] = val.source_;
    j["source_id"] = val.source_id_;
    j["name"] = val.name_;
    j["postalcode"] = val.postalcode_;
    j["postalcode_gid"] = val.postalcode_gid_;
    j["confidence"] = val.confidence_;
    j["distance"] = val.distance_;
    j["accuracy"] = val.accuracy_;
    j["country"] = val.country_;
    j["country_gid"] = val.country_gid_;
    j["country_a"] = val.country_a_;
    j["region"] = val.region_;
    j["region_gid"] = val.region_gid_;
    j["localadmin"] = val.localadmin_;
    j["localadmin_gid"] = val.localadmin_gid_;
    j["locality"] = val.locality_;
    j["locality_gid"] = val.locality_gid_;
    j["neighbourhood"] = val.neighbourhood_;
    j["neighbourhood_gid"] = val.neighbourhood_gid_;
    j["label"] = val.label_;
    j["zones_"] = val.zones_;
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

template <class T>
struct features {
  explicit features(const geometry_geocoding& geoArg,
                    const std::string& typeArg, const T& propertiesArg)
      : geo_(std::move(geoArg)),
        type_(std::move(typeArg)),
        properties(std::move(propertiesArg)){};

  void from_json(const nh::json& j, features& val) {
    nh::json geoJson = j.at("geometry");
    val.geo_.from_json(geoJson, val.geo_);

    j.at("type").get_to(val.type_);

    nh::json propertiesJson = j.at("properties");

    val.properties.from_json(propertiesJson, val.properties);
  };

  void to_json(nh::json& j, features& val) {
    nh::json geoJson;
    val.geo_.to_json(geoJson, val.geo_);
    j["geometry"] = geoJson;

    j["type"] = val.type_;

    nh::json propertiesJson;
    val.properties.to_json(propertiesJson, val.properties);
    j["properties"] = propertiesJson;
  };

  geometry_geocoding geo_;
  std::string type_;
  T properties;
};

void fillInfo(const StationGuesserResponse* station_res) {}

std::vector<features<properties_geocoding_osm>> geocodingAddressViaPosition(
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
  auto const g_response = motis_call(make_msg(mc));
  auto const g_res_val = g_response->val();

  auto const station_res = motis_content(StationGuesserResponse, g_res_val);

  auto const station_guesse = *station_res->guesses()->begin();
  mc.create_and_finish(
      MsgContent_AddressRequest,
      CreateAddressRequest(mc, mc.CreateString(station_guesse->name())).Union(),
      "/address");
  auto const a_response = motis_call(make_msg(mc));
  auto const a_res_val = a_response->val();

  auto const address_res = motis_content(AddressResponse, a_res_val);

  std::vector<features<properties_geocoding_osm>> features_vec;
  for (auto const a : *address_res->guesses()) {
    properties_geocoding_osm properties(
        station_guesse->id()->str(), "unknown_extentArg", "countryArg",
        "cityArg", "countrycodeArg", "countyArg", a->type()->str(),
        "osm_typeArg", "osm_keyArg", "housenumberArg", "streetArg",
        "districtArg", "osm_valueArg", "regionArg", "postalcodeArg",
        "localityArg", "labelArg", "nameArg", "layerArg", 1, "sourceArg");
    geometry_geocoding geometry(a->pos()->lat(), a->pos()->lng(),
                                a->type()->str());
    features<properties_geocoding_osm> feature(geometry, a->type()->str(),
                                               properties);
    features_vec.push_back(feature);
  }

  return features_vec;
}

void geocoding::init(motis::module::registry& reg) {
  reg.register_op(
      "/geocoding/reverse",
      [](mm::msg_ptr const& msg) {
        std::cout << msg->to_string() << std::endl;
        std::cout << msg->get()->content_type() << std::endl;

        //        auto const req = motis_content(HTTPRequest, msg);

        //        std::cout << req->path()->str() << std::endl;
        //        for (auto const& h : *req->headers()) {
        //          std::cout << h->name() << ": " << h->value() << std::endl;
        //        }
        //        auto response = req->content()->str();
        //        auto station_msg =
        //        geocodingAddressViaPosition(req->content()->str()); auto const
        //        station_res =
        //            motis_content(StationGuesserResponse, station_msg);

        //        std::cout << req->path()->str() << std::endl;
        //        auto response = req->content()->str();
        //        auto station_msg =
        //        geocodingAddressViaPosition(req->content()->str()); auto const
        //        station_res =
        //            motis_content(StationGuesserResponse, station_msg);

        //        std::vector<features<properties_geocoding_digi>> features_vec;
        //        properties_geocoding_digi pr("id", "gid", "layer");
        //        geometry_geocoding ge(1.0, 1.0, "type");
        //        features f1(ge, "saooo", pr);
        //
        //        features_vec.push_back(f1);
        //
        //        nh::json jsonArray = nh::json::array();
        //
        //        for (auto feature : features_vec) {
        //          nh::json featureJson;
        //          feature.to_json(featureJson, feature);
        //          jsonArray.push_back(featureJson);
        //        }
        //
        //        nh::json jsonObject;
        //        jsonObject["features"] = jsonArray;

        auto from =
            "{\n"
            "\t\"features\": [\n"
            "\t\t{\n"
            "\t\t\t\"geometry\": {\n"
            "\t\t\t\t\"coordinates\": [\n"
            "\t\t\t\t\t6.073154,\n"
            "\t\t\t\t\t50.780919999999995\n"
            "\t\t\t\t],\n"
            "\t\t\t\t\"type\": \"Point\"\n"
            "\t\t\t},\n"
            "\t\t\t\"type\": \"Feature\",\n"
            "\t\t\t\"properties\": {\n"
            "\t\t\t\t\"osm_id\": 25416882,\n"
            "\t\t\t\t\"extent\": [\n"
            "\t\t\t\t\t6.073154,\n"
            "\t\t\t\t\t50.780919999999995\n"
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
            "\t\t\t\t\"name\": \"Adalbertsteinweg\",\n"
            "\t\t\t\t\"region\": \"Nordrhein-Westfalen\",\n"
            "\t\t\t\t\"postalcode\": \"52066\",\n"
            "\t\t\t\t\"locality\": \"Aachen\",\n"
            "\t\t\t\t\"label\": \"Adalbertsteinweg, 52066 Aachen\",\n"
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
            "\t\t\t\t\t6.0971079999999995,\n"
            "\t\t\t\t\t50.772912999999996\n"
            "\t\t\t\t],\n"
            "\t\t\t\t\"type\": \"Point\"\n"
            "\t\t\t},\n"
            "\t\t\t\"type\": \"Feature\",\n"
            "\t\t\t\"properties\": {\n"
            "\t\t\t\t\"osm_id\": 25416882,\n"
            "\t\t\t\t\"extent\": [\n"
            "\t\t\t\t\t6.0971079999999995,\n"
            "\t\t\t\t\t50.772912999999996\n"
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
            "\t\t\t\t\"name\": \"Augustastraße\",\n"
            "\t\t\t\t\"region\": \"Nordrhein-Westfalen\",\n"
            "\t\t\t\t\"postalcode\": \"52070\",\n"
            "\t\t\t\t\"locality\": \"Aachen\",\n"
            "\t\t\t\t\"label\": \"Augustastraße 9, 52070 Aachen\",\n"
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
        std::cout << msg->to_string() << std::endl;
        std::cout << msg->get()->content_type() << std::endl;

        //        auto const req = motis_content(HTTPRequest, msg);

        //        std::cout << req->path()->str() << std::endl;
        //        for (auto const& h : *req->headers()) {
        //          std::cout << h->name() << ": " << h->value() << std::endl;
        //        }
        //        auto response = req->content()->str();
        //        auto station_msg =
        //        geocodingAddressViaPosition(req->content()->str()); auto const
        //        station_res =
        //            motis_content(StationGuesserResponse, station_msg);

        //        std::cout << req->path()->str() << std::endl;
        //        auto response = req->content()->str();
        //        auto station_msg =
        //        geocodingAddressViaPosition(req->content()->str()); auto const
        //        station_res =
        //            motis_content(StationGuesserResponse, station_msg);

        //        std::vector<features<properties_geocoding_digi>> features_vec;
        //        properties_geocoding_digi pr("id", "gid", "layer");
        //        geometry_geocoding ge(1.0, 1.0, "type");
        //        features f1(ge, "saooo", pr);
        //
        //        features_vec.push_back(f1);
        //
        //        nh::json jsonArray = nh::json::array();
        //
        //        for (auto feature : features_vec) {
        //          nh::json featureJson;
        //          feature.to_json(featureJson, feature);
        //          jsonArray.push_back(featureJson);
        //        }
        //
        //        nh::json jsonObject;
        //        jsonObject["features"] = jsonArray;

        auto from =
            "{\n"
            "\t\"features\": [\n"
            "\t\t{\n"
            "\t\t\t\"geometry\": {\n"
            "\t\t\t\t\"coordinates\": [\n"
            "\t\t\t\t\t6.073154,\n"
            "\t\t\t\t\t50.780919999999995\n"
            "\t\t\t\t],\n"
            "\t\t\t\t\"type\": \"Point\"\n"
            "\t\t\t},\n"
            "\t\t\t\"type\": \"Feature\",\n"
            "\t\t\t\"properties\": {\n"
            "\t\t\t\t\"osm_id\": 25416882,\n"
            "\t\t\t\t\"extent\": [\n"
            "\t\t\t\t\t6.073154,\n"
            "\t\t\t\t\t50.780919999999995\n"
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
            "\t\t\t\t\"name\": \"Adalbertsteinweg\",\n"
            "\t\t\t\t\"region\": \"Nordrhein-Westfalen\",\n"
            "\t\t\t\t\"postalcode\": \"52066\",\n"
            "\t\t\t\t\"locality\": \"Aachen\",\n"
            "\t\t\t\t\"label\": \"Adalbertsteinweg, 52066 Aachen\",\n"
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
            "\t\t\t\t\t6.0971079999999995,\n"
            "\t\t\t\t\t50.772912999999996\n"
            "\t\t\t\t],\n"
            "\t\t\t\t\"type\": \"Point\"\n"
            "\t\t\t},\n"
            "\t\t\t\"type\": \"Feature\",\n"
            "\t\t\t\"properties\": {\n"
            "\t\t\t\t\"osm_id\": 25416882,\n"
            "\t\t\t\t\"extent\": [\n"
            "\t\t\t\t\t6.0971079999999995,\n"
            "\t\t\t\t\t50.772912999999996\n"
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
            "\t\t\t\t\"name\": \"Lothringerstraße\",\n"
            "\t\t\t\t\"region\": \"Nordrhein-Westfalen\",\n"
            "\t\t\t\t\"postalcode\": \"52070\",\n"
            "\t\t\t\t\"locality\": \"Aachen\",\n"
            "\t\t\t\t\"label\": \"Lothringerstraße, 52070 Aachen\",\n"
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