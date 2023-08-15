cmake_minimum_required(VERSION 3.10)
project(motis)

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/otp/otpSchema.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/NodeObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/PlaceInterfaceObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/AlertEntityObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/AgencyObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/AlertObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/OpeningHoursObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/BikeParkObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/VehicleParkingObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/VehicleParkingSpacesObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/BikeRentalStationObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/VehicleRentalStationObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/RentalVehicleObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/BikeRentalStationUrisObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/VehicleRentalUrisObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/RentalVehicleTypeObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/CarParkObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/ClusterObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/CoordinatesObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/debugOutputObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/DepartureRowObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/elevationProfileComponentObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/fareObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/fareComponentObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/FeedObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/GeometryObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/StopGeometriesObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/RoutingErrorObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/RiderCategoryObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/FareMediumObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/FareProductUseObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/FareProductObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/ItineraryObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/CurrencyObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/MoneyObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/RideHailingProviderObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/RideHailingEstimateObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/LegObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/LocalTimeSpanObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/LocalTimeSpanDateObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/PageInfoObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/VehiclePositionObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/StopRelationshipObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/PatternObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/ContactInfoObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/BookingTimeObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/BookingInfoObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/PlaceObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/placeAtDistanceObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/placeAtDistanceConnectionObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/placeAtDistanceEdgeObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/PlanObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/QueryTypeObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/RouteObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/RouteTypeObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/serviceTimeRangeObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/stepObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/StopObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/stopAtDistanceObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/stopAtDistanceConnectionObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/stopAtDistanceEdgeObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/StopOnRouteObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/StopOnTripObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/StoptimeObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/StoptimesInPatternObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/TicketTypeObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/TranslatedStringObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/TripObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/SystemNoticeObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/UnknownObject.cpp
    COMMAND
        ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:cppgraphqlgen::schemagen>
            ${CMAKE_CURRENT_SOURCE_DIR}/schema.otp.graphql
            --source-dir ${CMAKE_CURRENT_BINARY_DIR}/otp
            --header-dir ${CMAKE_CURRENT_BINARY_DIR}/otp
            otp otp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/schema.otp.graphql
)

add_library(motis-otp-graphql
        ${CMAKE_CURRENT_BINARY_DIR}/otp/otpSchema.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/NodeObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/PlaceInterfaceObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/AlertEntityObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/AgencyObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/AlertObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/OpeningHoursObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/BikeParkObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/VehicleParkingObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/VehicleParkingSpacesObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/BikeRentalStationObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/VehicleRentalStationObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/RentalVehicleObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/BikeRentalStationUrisObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/VehicleRentalUrisObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/RentalVehicleTypeObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/CarParkObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/ClusterObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/CoordinatesObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/debugOutputObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/DepartureRowObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/elevationProfileComponentObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/fareObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/fareComponentObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/FeedObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/GeometryObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/StopGeometriesObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/RoutingErrorObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/RiderCategoryObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/FareMediumObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/FareProductUseObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/FareProductObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/ItineraryObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/CurrencyObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/MoneyObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/RideHailingProviderObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/RideHailingEstimateObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/LegObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/LocalTimeSpanObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/LocalTimeSpanDateObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/PageInfoObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/VehiclePositionObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/StopRelationshipObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/PatternObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/ContactInfoObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/BookingTimeObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/BookingInfoObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/PlaceObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/placeAtDistanceObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/placeAtDistanceConnectionObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/placeAtDistanceEdgeObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/PlanObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/QueryTypeObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/RouteObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/RouteTypeObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/serviceTimeRangeObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/stepObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/StopObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/stopAtDistanceObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/stopAtDistanceConnectionObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/stopAtDistanceEdgeObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/StopOnRouteObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/StopOnTripObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/StoptimeObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/StoptimesInPatternObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/TicketTypeObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/TranslatedStringObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/TripObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/SystemNoticeObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/UnknownObject.cpp
)
target_link_libraries(motis-otp-graphql cppgraphqlgen::graphqlservice)
target_include_directories(motis-otp-graphql PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB_RECURSE motis-graphql-files src/*)

add_library(motis-graphql STATIC ${motis-graphql-files})
target_include_directories(motis-graphql PUBLIC include)
target_compile_features(motis-graphql PUBLIC cxx_std_20)
target_link_libraries(motis-graphql motis-module motis-otp-graphql cppgraphqlgen::graphqljson)
target_compile_options(motis-graphql PRIVATE ${MOTIS_CXX_FLAGS})
