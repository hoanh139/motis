cmake_minimum_required(VERSION 3.10)
project(motis)

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/otp/TripObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/QueryTypeObject.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/otp/otpSchema.cpp
    COMMAND
        ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:cppgraphqlgen::schemagen>
            ${CMAKE_CURRENT_SOURCE_DIR}/schema.otp.graphql
            --source-dir ${CMAKE_CURRENT_BINARY_DIR}/otp
            --header-dir ${CMAKE_CURRENT_BINARY_DIR}/otp
            otp otp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/schema.otp.graphql
)

add_library(motis-otp-graphql
    ${CMAKE_CURRENT_BINARY_DIR}/otp/TripObject.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/otp/QueryTypeObject.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/otp/otpSchema.cpp
)
target_link_libraries(motis-otp-graphql cppgraphqlgen::graphqlservice)
target_include_directories(motis-otp-graphql PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB_RECURSE motis-graphql-files src/*.cc)
add_library(motis-graphql STATIC ${motis-graphql-files})
target_include_directories(motis-graphql PUBLIC include)
target_compile_features(motis-graphql PUBLIC cxx_std_20)
target_link_libraries(motis-graphql motis-module motis-otp-graphql cppgraphqlgen::graphqljson)
target_compile_options(motis-graphql PRIVATE ${MOTIS_CXX_FLAGS})
