#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomReviewedLocalStream.cmake
#
# PURPOSE:
# Compose approved local streaming with durable workspace execution and native regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
target_sources(umicom_ai_workspace PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_workspace/reviewed_stream.c")
if(BUILD_TESTING)
    umicom_local_delta_test_target(umicom-local-provider-settings-offline-test test_local_provider_settings_offline.c)
    target_link_libraries(umicom-local-provider-settings-offline-test PRIVATE Umicom::ai_workspace)
    umicom_local_delta_case(framework.ai_workspace.local_provider_settings_offline umicom-local-provider-settings-offline-test)
    umicom_local_delta_test_target(umicom-local-provider-settings-test test_local_provider_settings.c)
    target_link_libraries(umicom-local-provider-settings-test PRIVATE Umicom::ai_workspace)
    umicom_local_delta_case(framework.ai_workspace.local_provider_settings umicom-local-provider-settings-test)
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-local-provider-settings-offline-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai_workspace/local_chat_provider.c")
    endif()
    if(UMICOM_AI_WORKSPACE_HTTP_BUILT)
        umicom_local_delta_test_target(umicom-reviewed-local-stream-test test_reviewed_stream.c)
        target_sources(umicom-reviewed-local-stream-test PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_workspace/local_http_peer.c")
        target_link_libraries(umicom-reviewed-local-stream-test PRIVATE
            Umicom::ai_workspace Umicom::web Umicom::platform)
        if(WIN32)
            target_link_libraries(umicom-reviewed-local-stream-test PRIVATE ws2_32)
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(umicom-reviewed-local-stream-test FILES
                "${CMAKE_CURRENT_LIST_DIR}/../src/ai_workspace/reviewed_stream.c")
        endif()
        foreach(_case IN ITEMS arguments approval network-denied unsupported abandon cancel-before
                policy-change binding-change start-failure wrong-owner success grounded citation overflow
                truncated timeout cancel token-cancel late-cancel try-finish join-failure release-failure publication-failure)
            set(_name "framework.ai_workspace.reviewed_stream.${_case}")
            umicom_local_delta_case("${_name}" umicom-reviewed-local-stream-test "${_case}"
                "${CMAKE_CURRENT_BINARY_DIR}/reviewed-stream-${_case}.sqlite")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 30
                LABELS "framework;ai-workspace;stream;review;ownership;persistence")
        endforeach()
    endif()
endif()
