# Umicom Framework
# File: cmake/UmicomAiConversationTranscript.cmake
#
# PURPOSE:
# Compose shared conversation transcript formatting and explicit native regression cases.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT


include_guard(GLOBAL)
if(NOT TARGET umicom_ai)
    message(FATAL_ERROR "Conversation transcripts require the existing Framework AI target")
endif()
# Keep projection inside the provider-neutral archive service; no web toolkit or
# application-specific persistence implementation is needed by SDK consumers.
target_sources(umicom_ai PRIVATE src/ai/conversation_transcript.c)
if(BUILD_TESTING)
    add_executable(umicom-ai-conversation-transcript-test tests/test_ai_conversation_transcript.c)
    target_link_libraries(umicom-ai-conversation-transcript-test PRIVATE Umicom::ai)
    set_target_properties(umicom-ai-conversation-transcript-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-conversation-transcript-test)
    umicom_apply_sanitizers(umicom-ai-conversation-transcript-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ai-conversation-transcript-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-ai-conversation-transcript-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai/conversation_archive_fixture.h"
            "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/ai/conversation_transcript.h")
    endif()
    foreach(_case IN ITEMS golden dialogue system-tool range empty-range empty-text names hidden-names title-fences message-fences name-fences unicode line-endings control-title control-message control-name unicode-title unicode-message unicode-name excluded-encoding zero-mask unknown-mask range-overflow start-overflow count-overflow unterminated bad-role bad-classification invalid-hidden-record null-record null-options null-output null-size short-buffer zero-capacity exact-capacity maximum tail-bytes metadata-omitted)
        set(_name "framework.ai.conversation.transcript.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-conversation-transcript-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;ai;conversation;transcript;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-conversation-transcript-test)
        endif()
    endforeach()
endif()
