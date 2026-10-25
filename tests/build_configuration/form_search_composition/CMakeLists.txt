# Umicom Framework
# File: tests/build_configuration/form_search_composition/CMakeLists.txt
# PURPOSE: Configure real search/form modules together to detect target collisions.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomFormSearchComposition LANGUAGES C)
include(CTest)
foreach(required IN ITEMS ROOT FIXTURE_ORDER)
    if(NOT DEFINED ${required})
        message(FATAL_ERROR "Missing ${required}")
    endif()
endforeach()

# Only configuration is exercised here. Real module source lists and test
# registrations are used; dependency implementation is qualified separately.
add_library(umicom_base STATIC)
add_library(umicom_application STATIC)
add_library(umicom_form_records STATIC)
add_library(umicom_data INTERFACE)
add_library(umicom_platform INTERFACE)
add_library(Umicom::base ALIAS umicom_base)
add_library(Umicom::application ALIAS umicom_application)
add_library(Umicom::form_records ALIAS umicom_form_records)
add_library(Umicom::data ALIAS umicom_data)
add_library(Umicom::platform ALIAS umicom_platform)
function(umicom_apply_warnings target)
endfunction()
function(umicom_apply_sanitizers target)
endfunction()
set(modules UmicomFormText UmicomSearchReaderChecks
    UmicomStructuredFormSearch UmicomStructuredFormSearchDiscovery)
if(FIXTURE_ORDER STREQUAL "reverse")
    list(REVERSE modules)
elseif(NOT FIXTURE_ORDER STREQUAL "forward")
    message(FATAL_ERROR "Unknown include order")
endif()
# A second include verifies the modules' guards without concealing a collision
# between two different owning modules.
foreach(pass RANGE 1 2)
    foreach(module IN LISTS modules)
        include("${ROOT}/cmake/${module}.cmake")
    endforeach()
endforeach()
set(expected_targets umicom-form-ascii-test umicom-form-text-test
    umicom-search-reader-test umicom-structured-form-filter-filter-test
    umicom-structured-form-filter-query-test umicom-structured-form-filter-text-test
    umicom-structured-form-search-resolver-test
    umicom-structured-form-search-reader-test
    umicom-structured-form-search-catalogue-test)
foreach(target IN LISTS expected_targets)
    if(BUILD_TESTING AND NOT TARGET ${target})
        message(FATAL_ERROR "Expected test executable was not registered: ${target}")
    elseif(NOT BUILD_TESTING AND TARGET ${target})
        message(FATAL_ERROR "Test executable registered with testing disabled: ${target}")
    endif()
endforeach()
if(BUILD_TESTING)
    get_target_property(form_source umicom-form-text-test SOURCES)
    get_target_property(filter_source umicom-structured-form-filter-text-test SOURCES)
    get_target_property(reader_source umicom-search-reader-test SOURCES)
    get_target_property(discovery_source umicom-structured-form-search-reader-test SOURCES)
    if(NOT "${form_source}" MATCHES "test_structured_form_text[.]c$"
       OR NOT "${filter_source}" MATCHES "test_structured_form_filter_text[.]c$"
       OR NOT "${reader_source}" MATCHES "test_search_reader[.]c$"
       OR NOT "${discovery_source}" MATCHES "test_structured_form_search_reader[.]c$")
        message(FATAL_ERROR "A test target resolved to another service's source")
    endif()
    foreach(test IN ITEMS framework.structured_forms.text.decimal
            framework.search.reader.owned
            framework.structured_forms.discovery.reader.memory.decode)
        if(NOT TEST ${test})
            message(FATAL_ERROR "Existing test name was lost: ${test}")
        endif()
    endforeach()
endif()
