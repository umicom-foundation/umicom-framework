# Umicom Framework
# File: tests/sdk_composition/consumer/CMakeLists.txt
#
# PURPOSE:
#   Exercise consumer helper target construction with declared metadata fixtures, without building or qualifying an SDK binary.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

cmake_minimum_required(VERSION 3.24)
project(UmicomCompositionContract LANGUAGES C)
include(CTest)
include(GNUInstallDirs)
set(CMAKE_C_STANDARD 23)
# This fixture is copied to a build-owned directory before configuration.
# It generates only its own dummy application files. No executable is built.
foreach(_source src/application.c src/runtime.c src/readiness.c src/workspace_commands.c
    src/product.c tests/test_runtime.c tests/test_readiness_runtime.c
    tests/test_workspace_commands.c tests/test_product.c)
    get_filename_component(_parent "${CMAKE_CURRENT_SOURCE_DIR}/${_source}" DIRECTORY)
    file(MAKE_DIRECTORY "${_parent}")
    file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/${_source}" "/* Umicom composition configuration fixture; no runtime qualification. */\nint main(void) { return 0; }\n")
endforeach()
add_library(Umicom::application INTERFACE IMPORTED)
if(CASE STREQUAL "source")
    add_library(fixture_creative INTERFACE)
    add_library(Umicom::creative_workspace ALIAS fixture_creative)
    include("${FRAMEWORK}/cmake/UmicomApplicationComposition.cmake")
    set(_expected_example "${FRAMEWORK}/examples/creative_workspace_main.c")
else()
    if(NOT CASE STREQUAL "missing-component")
        add_library(Umicom::creative_workspace INTERFACE IMPORTED)
    endif()
    set(UMICOM_FRAMEWORK_CREATIVE_EXAMPLE_SOURCE "${SDK_FIXTURE}/examples/creative_workspace_main.c")
    if(CASE STREQUAL "missing-example")
        set(UMICOM_FRAMEWORK_CREATIVE_EXAMPLE_SOURCE "${SDK_FIXTURE}/examples/missing.c")
    endif()
    include("${SDK_FIXTURE}/cmake/UmicomApplicationComposition.cmake")
    set(_expected_example "${UMICOM_FRAMEWORK_CREATIVE_EXAMPLE_SOURCE}")
endif()
add_library(fixture_product STATIC src/application.c)
umicom_extend_thin_application_runtime(NAME media TARGET fixture_product)
umicom_add_application_productisation_contribution(NAME media TARGET fixture_contribution
    SOURCE src/product.c TEST_SOURCE tests/test_product.c)
set(_profile media)
if(CASE STREQUAL "unknown-profile")
    set(_profile unknown)
endif()
umicom_attach_creative_workspace(NAME "${_profile}" TARGET fixture_product)
if(CASE STREQUAL "duplicate")
    umicom_attach_creative_workspace(NAME media TARGET fixture_product)
endif()
get_target_property(_profile_result fixture_product UMICOM_CREATIVE_WORKSPACE_PROFILE)
get_target_property(_sources fixture_product SOURCES)
list(LENGTH _sources _source_count)
get_target_property(_example_sources umicom-media-creative-workspace-example SOURCES)
file(REAL_PATH "${_expected_example}" _expected_real)
file(REAL_PATH "${_example_sources}" _actual_real)
if(NOT _profile_result STREQUAL "media" OR NOT _source_count EQUAL 4 OR
   NOT _expected_real STREQUAL _actual_real OR NOT TARGET fixture_contribution)
    message(FATAL_ERROR "Consumer composition properties differ from the selected source owner")
endif()
get_property(_tests DIRECTORY PROPERTY TESTS)
foreach(_test media.module.runtime media.module.readiness_runtime media.module.workspace_commands
    media.module.productisation media.creative_workspace.example)
    if(NOT _test IN_LIST _tests)
        message(FATAL_ERROR "Missing application contract test: ${_test}")
    endif()
endforeach()
message(STATUS "Composition contract accepted; no Framework binary or application was built")
