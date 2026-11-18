#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/media_generation/http/CMakeLists.txt
#
# PURPOSE:
#   Build the real provider transport against deterministic curl call doubles.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.25)
project(UmicomProviderTransportChecks LANGUAGES C)
include(CTest)
find_package(PkgConfig REQUIRED)
pkg_check_modules(TRANSPORT_CURL REQUIRED IMPORTED_TARGET libcurl)
get_filename_component(_framework "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

# The test includes the production HTTP adapter with curl calls replaced before
# preprocessing. Its remaining dependencies are the real cancellation and
# secret-erasure services. No provider socket or local credential store is used.
add_executable(umicom-heygen-http-test
    "../test_heygen_http.c"
    "${_framework}/src/platform/cancellation.c"
    "${_framework}/src/security/secrets.c")
target_include_directories(umicom-heygen-http-test PRIVATE "${_framework}/include")
target_link_libraries(umicom-heygen-http-test PRIVATE PkgConfig::TRANSPORT_CURL)
target_compile_definitions(umicom-heygen-http-test PRIVATE UMICOM_HEYGEN_HAS_HTTP=1)
target_compile_features(umicom-heygen-http-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-heygen-http-test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
foreach(_case IN ITEMS get post redirect denied rate-limit timeout cancel overflow option-failure
    init-failure easy-failure header-accept-failure header-key-failure header-content-failure header-identity-failure transfer-error info-failure response-low response-high status-first status-last empty cancel-before cancel-write multiply-overflow chunked chunk-overflow exact-capacity)
    add_test(NAME framework.heygen.http.${_case} COMMAND umicom-heygen-http-test ${_case})
    set_tests_properties(framework.heygen.http.${_case} PROPERTIES TIMEOUT 20 LABELS "framework;heygen;http;privacy;regression")
endforeach()
