# Umicom Framework generated-source configure fixture.
# Sammy Hegab, Umicom Foundation. MIT licence.
cmake_minimum_required(VERSION 3.24)
project(GeneratedApplicationRegistration LANGUAGES C)
include(CTest)

# This fixture checks CMake registration, not linking or an installed SDK.
# Supplying just the expected target keeps the configuration boundary explicit.
add_library(Umicom::base INTERFACE IMPORTED)
include("${FRAMEWORK_SOURCE}/cmake/UmicomApplicationModules.cmake")
umicom_discover_application_modules(ROOT "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX FOUND ALL)
if(NOT FOUND_PATHS STREQUAL "applications/weather")
    message(FATAL_ERROR "The generated source was not discovered")
endif()
umicom_add_application_module(UMICOM_APPLICATIONS_BUILD_WEATHER
    applications/weather "Weather" weather)
if(NOT TARGET umicom-weather OR NOT TARGET umicom_weather_model OR
        NOT TARGET umicom-weather-model-test)
    message(FATAL_ERROR "Missing generated application targets")
endif()
get_property(_tests DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/applications/weather" PROPERTY TESTS)
if(NOT "weather.model" IN_LIST _tests OR NOT "weather.console" IN_LIST _tests)
    message(FATAL_ERROR "Generated tests were not registered")
endif()
get_property(_components GLOBAL PROPERTY UMICOM_SUITE_COMPONENT_IDS)
if(NOT "weather" IN_LIST _components)
    message(FATAL_ERROR "Generated installer component was not registered")
endif()
