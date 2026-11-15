#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLocalStreamExchange.cmake
#
# PURPOSE:
# Compose the blocking native streaming exchange and callback-boundary regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
target_sources(umicom_ai_workspace PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/ai_workspace/local_http_stream.c")
if(BUILD_TESTING)
    target_sources(umicom-local-chat-stream-offline-test PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../src/ai_workspace/local_http_stream.c")
    if(UMICOM_AI_WORKSPACE_HTTP_BUILT)
        umicom_local_delta_test_target(umicom-local-http-stream-test test_local_http_stream.c)
        target_compile_definitions(umicom-local-http-stream-test PRIVATE UMICOM_AI_WORKSPACE_HAS_HTTP=1)
        target_link_libraries(umicom-local-http-stream-test PRIVATE Umicom::ai_workspace
            Umicom::web PkgConfig::UMICOM_AI_LOCAL_CURL)
        # The test defines the exchange once by including its real source with
        # setup/perform intercepted; other workspace owners come from the archive.
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(umicom-local-http-stream-test FILES
                "${CMAKE_CURRENT_LIST_DIR}/../src/ai_workspace/local_http_stream.c")
        endif()
        foreach(_case IN ITEMS preconditions success fragmented wrong-type missing-type status-busy setup-policy timeout-before timeout-partial truncated-http cancel-before cancel-progress cancel-after-done missing-done early-done duplicate-done bad-json short-length bad-trailer body-limit header-overflow write-overflow)
            umicom_local_delta_case("framework.ai_workspace.http_stream.${_case}" umicom-local-http-stream-test "${_case}")
        endforeach()
    endif()
endif()
