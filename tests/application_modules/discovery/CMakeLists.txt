# Umicom Framework
# File: tests/application_modules/discovery/CMakeLists.txt
# Sammy Hegab, Umicom Foundation. MIT licence.
cmake_minimum_required(VERSION 3.24)
project(UmicomModuleDiscoveryFixture LANGUAGES NONE)
include(CTest)
include("${FRAMEWORK_SOURCE}/cmake/UmicomApplicationModules.cmake")

# The driver owns this isolated source tree. Its module install rule and CTest
# entry let this fixture observe production registration, not a mock catalogue.
set(_all "")
if(CASE STREQUAL "all" OR CASE STREQUAL "missing_all")
    set(_all ALL)
endif()
if(CASE STREQUAL "off" OR CASE STREQUAL "all")
    set(UMICOM_APPLICATIONS_BUILD_WEATHER OFF CACHE BOOL "Explicit selection")
elseif(CASE STREQUAL "enabled" OR CASE STREQUAL "duplicate_registration")
    set(UMICOM_APPLICATIONS_BUILD_WEATHER ON CACHE BOOL "Explicit selection")
endif()
if(CASE STREQUAL "reserved")
    set(_reserved UMICOM_APPLICATIONS_BUILD_WEATHER)
endif()
umicom_discover_application_modules(
    ROOT "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX FOUND ${_all}
    RESERVED_OPTIONS ${_reserved})
if(CASE STREQUAL "missing")
    if(NOT "applications/weather" IN_LIST FOUND_MISSING OR FOUND_PATHS)
        message(FATAL_ERROR "Missing module was silently treated as available")
    endif()
    return()
endif()
get_property(_inputs DIRECTORY PROPERTY CMAKE_CONFIGURE_DEPENDS)
if(NOT "${CMAKE_CURRENT_SOURCE_DIR}/.gitmodules" IN_LIST _inputs OR
   NOT "${CMAKE_CURRENT_SOURCE_DIR}/applications/weather/application.umicom.yaml" IN_LIST _inputs)
    message(FATAL_ERROR "Registry or manifest lacks automatic reconfigure tracking")
endif()
list(LENGTH FOUND_PATHS _count)
if(NOT _count EQUAL 1)
    message(FATAL_ERROR "Expected one registered module, got ${_count}")
endif()
string(SHA256 _key "applications/weather")
if(NOT FOUND_${_key}_option STREQUAL "UMICOM_APPLICATIONS_BUILD_WEATHER" OR
   NOT FOUND_${_key}_component STREQUAL "UmicomWeather")
    message(FATAL_ERROR "Descriptor derivation changed")
endif()
foreach(_path IN LISTS FOUND_PATHS)
    string(SHA256 _key "${_path}")
    umicom_add_application_module("${FOUND_${_key}_option}" "${_path}"
        "${FOUND_${_key}_name}" "${FOUND_${_key}_component}")
endforeach()
get_property(_registered GLOBAL PROPERTY UMICOM_SUITE_APPLICATION_DIRECTORIES)
if(CASE STREQUAL "enabled" OR CASE STREQUAL "all" OR CASE STREQUAL "duplicate_registration")
    if(NOT TARGET weather-module OR NOT _registered STREQUAL "applications/weather")
        message(FATAL_ERROR "Selected module was not registered")
    endif()
    get_property(_component DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/applications/weather"
        PROPERTY UMICOM_APPLICATION_INSTALL_COMPONENT)
    if(NOT _component STREQUAL "UmicomWeather")
        message(FATAL_ERROR "Installer component was not inherited")
    endif()
    if(CASE STREQUAL "duplicate_registration")
        umicom_add_application_module(UMICOM_APPLICATIONS_BUILD_WEATHER
            applications/weather "Umicom Weather" UmicomWeather)
    endif()
else()
    if(TARGET weather-module OR _registered)
        message(FATAL_ERROR "Disabled module executed its CMake project")
    endif()
endif()
