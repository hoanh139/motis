//#include "motis/geocoding/geocoding.h"
//
//#include "motis/core/access/time_access.h"
//#include "motis/module/context/motis_call.h"
//#include "motis/module/message.h"
//#include "geo/polyline_format.h"
//
//namespace mm = motis::module;
//
//namespace motis::geocoding {
//
//geocoding::geocoding() : module("Geocoding", "geocoding") {}
//
//void geocoding::init(motis::module::registry& reg) {
//  reg.register_op(
//      "/geocoding",
//      [](mm::msg_ptr const& msg) {
//        auto const req = motis_content(HTTPRequest, msg);
//
//        std::shared_ptr<gql::service::Request> service =
//            std::make_shared<otp::Operations>(std::make_shared<Query>());
//
//        auto payload = gql::response::parseJSON(req->content()->str());
//        auto variablesItr = payload.find("variables"sv);
//        //        std::cout << "Content:" << req->content()->str() << std::endl;
//        if (variablesItr == payload.end() ||
//            variablesItr->second.type() != gql::response::Type::Map) {
//          throw std::runtime_error{"variables missing"};
//        }
//        auto variables = gql::response::Value{gql::response::Type::Map};
//        for (auto [k, v] : variablesItr->second) {
//          variables.emplace_back(std::move(k), std::move(v));
//        }
//
//        auto queryItr = payload.find("query"sv);
//        if (queryItr == payload.end() ||
//            queryItr->second.type() != gql::response::Type::String) {
//          throw std::runtime_error("query missing");
//        }
//
//        auto query = gql::peg::parseString(
//            queryItr->second.get<gql::response::StringType>());
//        auto const response = gql::response::toJSON(
//            service
//                ->resolve({.query = query,
//                           .operationName = "GetPlan"sv,
//                           .variables = std::move(variables)})
//                .get());
//
//        mm::message_creator mc;
//        mc.create_and_finish(
//            MsgContent_HTTPResponse,
//            CreateHTTPResponse(
//                mc, HTTPStatus_OK,
//                mc.CreateVector(std::vector<fbb::Offset<HTTPHeader>>{}),
//                mc.CreateString(response))
//                .Union());
//        return make_msg(mc);
//      },
//      {});
//}
//
//}  // namespace motis::graphql