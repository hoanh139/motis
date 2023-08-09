#include "motis/graphql/graphql.h"

#include <chrono>
#include "graphqlservice/JSONResponse.h"
#include "otp/QueryTypeObject.h"
#include "otp/TripObject.h"
#include "otp/otpSchema.h"

namespace mm = motis::module;
namespace fbb = flatbuffers;
namespace gql = graphql;
namespace otp = gql::otp;
namespace otpo = otp::object;
using namespace std::string_view_literals;

namespace motis::graphql {

struct Trip {
  gql::response::ScalarType getDeparture() const noexcept {
    return gql::response::ScalarType{"a"};
  }
  gql::response::ScalarType getArrival() const noexcept {
    return gql::response::ScalarType{"b"};
  }
};

struct Query {
  std::shared_ptr<otpo::Trip> getTrip(otp::Location const& from,
                                      otp::Location const& to) const noexcept {
    (void)from;
    (void)to;

    std::shared_ptr<otpo::Trip> result =
        std::make_shared<otpo::Trip>(std::make_shared<Trip>());

    return result;
  }  // namespace motis::graphql
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
                           .operationName = "GetTrip"sv,
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
