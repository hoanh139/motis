cmake_minimum_required(VERSION 3.10)
project(motis)

target_include_directories(motis-geocoding PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB_RECURSE motis-geocoding-files src/*)

add_library(motis-geocoding STATIC ${motis-geocoding-files})
target_include_directories(motis-geocoding PUBLIC include)
target_compile_features(motis-geocoding PUBLIC cxx_std_20)
target_link_libraries(motis-geocoding motis-module motis-tiles)
target_compile_options(motis-geocoding PRIVATE ${MOTIS_CXX_FLAGS})
