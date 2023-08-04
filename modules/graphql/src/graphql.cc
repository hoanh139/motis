#include "motis/graphql/graphql.h"

#include "graphqlservice/JSONResponse.h"
#include "otp/QueryTypeObject.h"
#include "otp/TripObject.h"
#include "otp/otpSchema.h"

namespace mm = motis::module;
namespace fbb = flatbuffers;
namespace gql = graphql;

namespace motis::graphql {

graphql::graphql() : module("GraphQL", "graphql") {}

void graphql::init(motis::module::registry& reg) {
  reg.register_op(
      "/graphql",
      [](mm::msg_ptr const& msg) {
        auto const req = motis_content(HTTPRequest, msg);

        auto const query = gql::peg::parseString(req->content()->str());
        gql::otp::Operations{std::make_shared<gql::otp::object::QueryType>(
            []() { return std::make_shared<gql::otp::object::Trip>(); })};

        mm::message_creator mc;
        mc.create_and_finish(
            MsgContent_HTTPResponse,
            CreateHTTPResponse(
                mc, HTTPStatus_OK,
                mc.CreateVector(std::vector<fbb::Offset<HTTPHeader>>{}),
                mc.CreateString(req->content()->str()))
                .Union());
        return make_msg(mc);
      },
      {});
}

}  // namespace motis::graphql
