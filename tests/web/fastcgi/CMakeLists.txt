#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/web/fastcgi/CMakeLists.txt
#
# PURPOSE:
#   Qualify the same FastCGI codecs through Framework and a focused build.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomFastCgiChecks LANGUAGES C)
    include(CTest)
    get_filename_component(_framework "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)
    add_library(umicom-fastcgi-wire STATIC
        "${_framework}/src/web/fastcgi.c" "${_framework}/src/web/fastcgi_reply.c"
        "${_framework}/src/web/fastcgi_exchange.c" "${_framework}/src/platform/cancellation.c"
        "${_framework}/src/web/fastcgi_response.c" "${_framework}/src/web/closed_response.c"
        "${_framework}/src/web/request_frame.c" "${_framework}/src/web/request.c"
        "${_framework}/src/web/response.c" "${_framework}/src/web/response_body.c"
        "${_framework}/src/web/header.c" "${_framework}/src/web/types.c"
        "${_framework}/src/web/fastcgi_application.c" "${_framework}/src/platform/path.c"
        "${_framework}/src/web/connection.c" "${_framework}/src/web/request_stream.c"
        "${_framework}/src/web/service.c"
        "${_framework}/src/web/router.c" "${_framework}/src/web/route.c"
        "${_framework}/src/web/middleware.c" "${_framework}/src/web/session.c"
        "${_framework}/src/web/origin.c" "${_framework}/src/web/endpoint.c"
        "${_framework}/src/web/metrics.c" "${_framework}/src/web/server_connection_native.c"
        "${_framework}/src/platform/clock.c")
    target_include_directories(umicom-fastcgi-wire PUBLIC "${_framework}/include")
    target_compile_features(umicom-fastcgi-wire PUBLIC c_std_23)
    set_target_properties(umicom-fastcgi-wire PROPERTIES C_EXTENSIONS OFF)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(umicom-fastcgi-wire PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    if(WIN32)
        target_link_libraries(umicom-fastcgi-wire PRIVATE ws2_32)
    endif()
    set(_fastcgi_owner umicom-fastcgi-wire)
else()
    set(_fastcgi_owner Umicom::web)
endif()

add_executable(umicom-fastcgi-wire-test test_fastcgi.c)
target_link_libraries(umicom-fastcgi-wire-test PRIVATE ${_fastcgi_owner})
target_compile_features(umicom-fastcgi-wire-test PRIVATE c_std_23)
set_target_properties(umicom-fastcgi-wire-test PROPERTIES C_EXTENSIONS OFF)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-fastcgi-wire-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-fastcgi-wire-test)
endif()

foreach(_case begin padded-input large-input empty-input short-output short-parameter long-parameter bad-parameter
        truncated-frame bad-header two-records response wrong-request shared-budget closed-stream end-status bad-end)
    set(_test "framework.web.fastcgi.${_case}")
    add_test(NAME "${_test}" COMMAND umicom-fastcgi-wire-test "${_case}")
    set_tests_properties("${_test}" PROPERTIES TIMEOUT 15 LABELS "framework;web;fastcgi;local-fixture")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners("${_test}" TARGETS umicom-fastcgi-wire-test)
    endif()
endforeach()

if((WIN32 OR UNIX) AND NOT EMSCRIPTEN)
    # Compile the actual supervisor for the standalone hosted fixture. Full
    # Framework builds already supply this owner through the web target.
    if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
        find_package(Threads REQUIRED)
        target_sources(umicom-fastcgi-wire PRIVATE
            "${_framework}/src/web/managed_worker.c"
            "${_framework}/src/base/status.c"
            "${_framework}/src/platform/process.c"
            "${_framework}/src/platform/process_search_path.c"
            "${_framework}/src/platform/process_environment.c"
            "${_framework}/src/platform/process_supervisor.c"
            "${_framework}/src/platform/output_file.c"
            "${_framework}/src/platform/threading.c")
        target_link_libraries(umicom-fastcgi-wire PRIVATE Threads::Threads)
        if(WIN32)
            target_link_libraries(umicom-fastcgi-wire PRIVATE shell32 user32)
        endif()
    endif()
    add_executable(umicom-web-managed-worker-test test_managed_worker.c)
    target_link_libraries(umicom-web-managed-worker-test PRIVATE ${_fastcgi_owner})
    target_compile_features(umicom-web-managed-worker-test PRIVATE c_std_23)
    set_target_properties(umicom-web-managed-worker-test PROPERTIES C_EXTENSIONS OFF)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(umicom-web-managed-worker-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-web-managed-worker-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-web-managed-worker-test)
    endif()
    foreach(_case invalid ready downstream stop-and-restart exit missing release)
        set(_test "framework.web.managed_worker.${_case}")
        add_test(NAME "${_test}" COMMAND umicom-web-managed-worker-test "${_case}"
            "$<TARGET_FILE:umicom-web-managed-worker-test>" "${CMAKE_CURRENT_BINARY_DIR}")
        set_tests_properties("${_test}" PROPERTIES TIMEOUT 20 LABELS "framework;web;fastcgi;managed-worker")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${_test}" TARGETS umicom-web-managed-worker-test)
        endif()
    endforeach()

    add_executable(umicom-fastcgi-loopback-test test_loopback_connector.c)
    target_link_libraries(umicom-fastcgi-loopback-test PRIVATE ${_fastcgi_owner})
    if(WIN32)
        target_link_libraries(umicom-fastcgi-loopback-test PRIVATE ws2_32)
    endif()
    target_compile_features(umicom-fastcgi-loopback-test PRIVATE c_std_23)
    set_target_properties(umicom-fastcgi-loopback-test PROPERTIES C_EXTENSIONS OFF)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(umicom-fastcgi-loopback-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-fastcgi-loopback-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-fastcgi-loopback-test)
    endif()
    foreach(_case config cancel-before closed-port deadline cancel-connected independent binary)
        set(_test "framework.web.fastcgi_loopback.${_case}")
        add_test(NAME "${_test}" COMMAND umicom-fastcgi-loopback-test "${_case}")
        set_tests_properties("${_test}" PROPERTIES TIMEOUT 15 LABELS "framework;web;fastcgi;local-fixture")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${_test}" TARGETS umicom-fastcgi-loopback-test)
        endif()
    endforeach()
endif()

add_executable(umicom-fastcgi-exchange-test test_exchange.c)
target_link_libraries(umicom-fastcgi-exchange-test PRIVATE ${_fastcgi_owner})
target_compile_features(umicom-fastcgi-exchange-test PRIVATE c_std_23)
set_target_properties(umicom-fastcgi-exchange-test PROPERTIES C_EXTENSIONS OFF)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-fastcgi-exchange-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-fastcgi-exchange-test)
endif()
foreach(_case fragmented coalesced binary-request bad-variable duplicate-variable outbound-budget complete-error
        worker-refusal premature-eof partial-write invalid-count cancellation receive-budget wire-budget trailing-frame)
    set(_test "framework.web.fastcgi_exchange.${_case}")
    add_test(NAME "${_test}" COMMAND umicom-fastcgi-exchange-test "${_case}")
    set_tests_properties("${_test}" PROPERTIES TIMEOUT 15 LABELS "framework;web;fastcgi;local-fixture")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners("${_test}" TARGETS umicom-fastcgi-exchange-test)
    endif()
endforeach()

add_executable(umicom-fastcgi-response-test test_response.c)
target_link_libraries(umicom-fastcgi-response-test PRIVATE ${_fastcgi_owner})
target_compile_features(umicom-fastcgi-response-test PRIVATE c_std_23)
set_target_properties(umicom-fastcgi-response-test PROPERTIES C_EXTENSIONS OFF)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-fastcgi-response-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-fastcgi-response-test)
endif()
foreach(_case binary unix-lines status-and-length repeated-cookie empty location implicit-location
        mixed-lines unterminated bare-carriage-return folded-field nul-header missing-type
        duplicate-type duplicate-status duplicate-length wrong-length length-list invalid-status
        interim-status body-forbidden reset-with-body trailer connection-framing transfer-framing
        header-budget field-budget value-budget length-budget failed-worker actual-exchange)
    set(_test "framework.web.fastcgi_response.${_case}")
    add_test(NAME "${_test}" COMMAND umicom-fastcgi-response-test "${_case}")
    set_tests_properties("${_test}" PROPERTIES TIMEOUT 15 LABELS "framework;web;fastcgi;local-fixture")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners("${_test}" TARGETS umicom-fastcgi-response-test)
    endif()
endforeach()

add_executable(umicom-fastcgi-application-test test_application.c)
target_link_libraries(umicom-fastcgi-application-test PRIVATE ${_fastcgi_owner})
target_compile_features(umicom-fastcgi-application-test PRIVATE c_std_23)
set_target_properties(umicom-fastcgi-application-test PROPERTIES C_EXTENSIONS OFF)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-fastcgi-application-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-fastcgi-application-test)
endif()
foreach(_case post get other-path relative-script server-name head duplicate-content-type
        malformed-query body-budget reply-budget cancel worker-exit worker-document)
    set(_test "framework.web.fastcgi_application.${_case}")
    add_test(NAME "${_test}" COMMAND umicom-fastcgi-application-test "${_case}")
    set_tests_properties("${_test}" PROPERTIES TIMEOUT 15 LABELS "framework;web;fastcgi;local-fixture")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners("${_test}" TARGETS umicom-fastcgi-application-test)
    endif()
endforeach()

add_executable(umicom-fastcgi-connector-test test_connector_route.c)
target_link_libraries(umicom-fastcgi-connector-test PRIVATE ${_fastcgi_owner})
target_compile_features(umicom-fastcgi-connector-test PRIVATE c_std_23)
set_target_properties(umicom-fastcgi-connector-test PROPERTIES C_EXTENSIONS OFF)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-fastcgi-connector-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-fastcgi-connector-test)
endif()
foreach(_case served repeat wrong-route head request-gate static-first missing-worker
        invalid-io worker-write worker-output cancel-before cancel-open)
    set(_test "framework.web.fastcgi_connector.${_case}")
    add_test(NAME "${_test}" COMMAND umicom-fastcgi-connector-test "${_case}")
    set_tests_properties("${_test}" PROPERTIES TIMEOUT 15 LABELS "framework;web;fastcgi;local-fixture")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners("${_test}" TARGETS umicom-fastcgi-connector-test)
    endif()
endforeach()
