#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLocalChatStream.cmake
#
# PURPOSE:
# Compose native SSE and AI text owners and register stateful provider regressions.
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
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_workspace/local_chat_stream.c")
# Reuse Web framing in the implementation. CMake carries a static library's
# private link dependency to consumers; no application reimplements SSE.
target_link_libraries(umicom_ai_workspace PRIVATE Umicom::web)
if(BUILD_TESTING)
    umicom_local_delta_test_target(umicom-local-chat-stream-test test_local_chat_stream.c)
    target_link_libraries(umicom-local-chat-stream-test PRIVATE Umicom::ai_workspace)
    foreach(_case IN ITEMS fragmented empty-answer length-answer inline-usage empty-content early-done changed-id changed-model usage-before-finish choice-usage-before-finish duplicate-finish text-after-finish role-after-finish empty-choice-after-finish choice-usage-after-finish duplicate-usage usage-after-inline-usage usage-changed-id duplicate-done text-after-done partial-after-done comment-after-done unsupported-event tool-call malformed invalid-utf8 no-finish no-done undelimited-done)
        umicom_local_delta_case("framework.ai_workspace.stream_sequence.${_case}"
            umicom-local-chat-stream-test "${_case}")
    endforeach()
    umicom_local_delta_test_target(umicom-local-chat-stream-boundary-test test_local_chat_stream_boundaries.c)
    target_link_libraries(umicom-local-chat-stream-boundary-test PRIVATE Umicom::ai_workspace)
    foreach(_case IN ITEMS arguments cancel-active cancel-marker transport-active transport-marker text-limit text-exact wire-limit wire-exact)
        umicom_local_delta_case("framework.ai_workspace.stream_boundary.${_case}"
            umicom-local-chat-stream-boundary-test "${_case}")
    endforeach()
    umicom_local_delta_test_target(umicom-local-chat-stream-offline-test test_local_chat_stream_offline.c)
    # Compile the real codec without its optional HTTP definition. The workspace
    # archive is excluded so it cannot satisfy these same private symbols.
    target_sources(umicom-local-chat-stream-offline-test PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../src/ai_workspace/local_chat_codec.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/ai_workspace/local_chat_stream.c")
    target_link_libraries(umicom-local-chat-stream-offline-test PRIVATE Umicom::ai Umicom::web)
    umicom_local_delta_case(framework.ai_workspace.stream_offline umicom-local-chat-stream-offline-test)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang" AND NOT APPLE AND NOT MSVC)
        umicom_local_delta_test_target(umicom-local-chat-stream-fault-test test_local_chat_stream_faults.c)
        target_link_libraries(umicom-local-chat-stream-fault-test PRIVATE Umicom::ai_workspace)
        target_link_options(umicom-local-chat-stream-fault-test PRIVATE
            -Wl,--wrap=UmiAiResponseStreamPush -Wl,--wrap=UmiAiResponseStreamFail)
        foreach(_case IN ITEMS committed-push committed-final failure-publication)
            umicom_local_delta_case("framework.ai_workspace.stream_fault.${_case}"
                umicom-local-chat-stream-fault-test "${_case}")
        endforeach()
    endif()
endif()
