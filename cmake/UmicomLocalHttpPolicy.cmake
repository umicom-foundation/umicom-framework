#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLocalHttpPolicy.cmake
#
# PURPOSE:
# Compose shared local HTTP policy and source-scoped option-failure regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
target_sources(umicom_ai_workspace PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/ai_workspace/local_http_policy.c")
if(BUILD_TESTING AND UMICOM_AI_WORKSPACE_HTTP_BUILT)
    umicom_local_delta_test_target(umicom-local-http-policy-test test_local_http_policy.c)
    # The test includes the implementation exactly once with only setopt
    # intercepted. Link no workspace archive and perform no network request.
    target_compile_definitions(umicom-local-http-policy-test PRIVATE UMICOM_AI_WORKSPACE_HAS_HTTP=1)
    target_link_libraries(umicom-local-http-policy-test PRIVATE Umicom::base PkgConfig::UMICOM_AI_LOCAL_CURL)
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-local-http-policy-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai_workspace/local_http_policy.c")
    endif()
    foreach(_case IN ITEMS arguments health chat stream fail-health fail-chat fail-stream)
        umicom_local_delta_case("framework.ai_workspace.http_policy.${_case}" umicom-local-http-policy-test "${_case}")
    endforeach()
endif()
