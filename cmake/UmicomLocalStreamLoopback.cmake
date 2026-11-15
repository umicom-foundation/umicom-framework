#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLocalStreamLoopback.cmake
#
# PURPOSE:
# Register native loopback qualification for the shared local streaming exchange.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
if(BUILD_TESTING AND UMICOM_AI_WORKSPACE_HTTP_BUILT)
    umicom_local_delta_test_target(umicom-local-http-stream-loopback-test test_local_http_stream_loopback.c)
    target_sources(umicom-local-http-stream-loopback-test PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_workspace/local_http_peer.c")
    target_link_libraries(umicom-local-http-stream-loopback-test PRIVATE Umicom::ai_workspace
        Umicom::web Umicom::platform PkgConfig::UMICOM_AI_LOCAL_CURL PkgConfig::UMICOM_AI_LOCAL_JSONC)
    if(WIN32)
        target_link_libraries(umicom-local-http-stream-loopback-test PRIVATE ws2_32)
    endif()
    foreach(_case IN ITEMS success fragmented chunked informational wrong-type missing-type status-busy redirect bad-json missing-done duplicate-done truncated-http timeout cancel-after-request proxy-bypass)
        set(_name "framework.ai_workspace.http_stream_loopback.${_case}")
        umicom_local_delta_case("${_name}" umicom-local-http-stream-loopback-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 20
            LABELS "framework;ai-workspace;stream;loopback;network")
    endforeach()
endif()
