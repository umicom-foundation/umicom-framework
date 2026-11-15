#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLocalStreamJobs.cmake
#
# PURPOSE:
# Compose public local stream jobs, explicit HTTP lifetime and lifecycle regressions.
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
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_workspace/local_transport.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_workspace/local_stream.c")
# Native request ownership directly calls Platform thread and mutex services.
# Declare that relationship for installed static consumers as well as tests.
target_link_libraries(umicom_ai_workspace PRIVATE Umicom::platform)
if(BUILD_TESTING)
    umicom_local_delta_test_target(umicom-local-transport-offline-test test_local_transport_offline.c)
    target_sources(umicom-local-transport-offline-test PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../src/ai_workspace/local_transport.c")
    target_link_libraries(umicom-local-transport-offline-test PRIVATE Umicom::platform)
    umicom_local_delta_case(framework.ai_workspace.local_transport_offline umicom-local-transport-offline-test)
    if(UMICOM_AI_WORKSPACE_HTTP_BUILT)
        umicom_local_delta_test_target(umicom-local-transport-test test_local_transport.c)
        target_compile_definitions(umicom-local-transport-test PRIVATE UMICOM_AI_WORKSPACE_HAS_HTTP=1)
        target_link_libraries(umicom-local-transport-test PRIVATE Umicom::platform PkgConfig::UMICOM_AI_LOCAL_CURL)
        foreach(_case IN ITEMS arguments allocation init-failure missing-features missing-version lifetime wrong-owner overflow two-runtimes)
            umicom_local_delta_case("framework.ai_workspace.local_transport.${_case}" umicom-local-transport-test "${_case}")
        endforeach()
        umicom_local_delta_test_target(umicom-local-stream-job-test test_local_stream_job.c)
        target_link_libraries(umicom-local-stream-job-test PRIVATE Umicom::ai_workspace Umicom::web Umicom::platform)
        foreach(_case IN ITEMS arguments success input-copy setup-failure transport-failure cancel-before cancel-running completed-before-join thread-start-failure join-failure release-failure wrong-owner)
            umicom_local_delta_case("framework.ai_workspace.local_stream_job.${_case}" umicom-local-stream-job-test "${_case}")
        endforeach()
        umicom_local_delta_test_target(umicom-local-stream-job-loopback-test test_local_stream_loopback.c)
        target_sources(umicom-local-stream-job-loopback-test PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_workspace/local_http_peer.c")
        target_link_libraries(umicom-local-stream-job-loopback-test PRIVATE Umicom::ai_workspace Umicom::web Umicom::platform)
        if(WIN32)
            target_link_libraries(umicom-local-stream-job-loopback-test PRIVATE ws2_32)
        endif()
        foreach(_case IN ITEMS success timeout cancel truncated)
            set(_name "framework.ai_workspace.local_stream_job_loopback.${_case}")
            umicom_local_delta_case("${_name}" umicom-local-stream-job-loopback-test "${_case}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 20
                LABELS "framework;ai-workspace;stream;loopback;lifetime")
        endforeach()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(umicom-local-transport-test FILES
                "${CMAKE_CURRENT_LIST_DIR}/../src/ai_workspace/local_transport.c")
            umicom_target_qualification_inputs(umicom-local-stream-job-test FILES
                "${CMAKE_CURRENT_LIST_DIR}/../src/ai_workspace/local_stream.c")
        endif()
    endif()
endif()
