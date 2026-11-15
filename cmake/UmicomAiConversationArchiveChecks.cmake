#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiConversationArchiveChecks.cmake
#
# PURPOSE:
# Register bounded conversation admission and checked revision regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
if(BUILD_TESTING)
    foreach(_suite IN ITEMS admission revision)
        set(_target "umicom-ai-conversation-${_suite}-test")
        add_executable("${_target}" "tests/test_ai_conversation_archive_${_suite}.c")
        target_link_libraries("${_target}" PRIVATE Umicom::ai)
        set_target_properties("${_target}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings("${_target}")
        umicom_apply_sanitizers("${_target}")
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs("${_target}" FILES
                "${CMAKE_CURRENT_LIST_DIR}/../tests/ai/conversation_archive_fixture.h")
        endif()
    endforeach()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-ai-conversation-revision-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai/conversation_archive.c")
    endif()
    foreach(_case IN ITEMS session provider model workspace title conversation message-name message-text
            empty-session empty-provider empty-model empty-workspace empty-title optional-empty
            last-terminators unused-message role-low role-high count classification-low classification-high
            input-revision-zero input-revision-max approval-flags query null-record)
        set(_name "framework.ai.conversation.admission.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-conversation-admission-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;ai;conversation;admission")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-conversation-admission-test)
    endforeach()
    foreach(_case IN ITEMS conditional-insert stale-replace missing-replace stale-remove remove-neighbour
            saturation-upsert saturation-remove last-revision capacity count zero-revision zero-record
            ahead-record duplicate stored-text capture missing-remove unrelated-edit)
        set(_name "framework.ai.conversation.revision.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-conversation-revision-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;ai;conversation;revision")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-conversation-revision-test)
    endforeach()

    add_executable(umicom-ai-conversation-parsing-test tests/test_ai_conversation_archive_parsing.c)
    target_link_libraries(umicom-ai-conversation-parsing-test PRIVATE Umicom::ai)
    set_target_properties(umicom-ai-conversation-parsing-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-conversation-parsing-test)
    umicom_apply_sanitizers(umicom-ai-conversation-parsing-test)
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-ai-conversation-parsing-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai/conversation_archive_fixture.h")
    endif()
    foreach(_case IN ITEMS valid crlf reordered no-final-newline escaped empty-messages
            maximum-messages optional-empty max-u64 zero-revision alias null-input null-output
            empty size-limit embedded-nul escaped-nul invalid-escape short-escape duplicate-metadata
            duplicate-message index-alias missing-metadata missing-message orphan-message
            missing-separator unknown-field bad-format numeric-sign numeric-space numeric-overflow
            numeric-trailing index-sign index-space index-range classification-range
            classification-alias message-count-range role-range field-overflow)
        set(_name "framework.ai.conversation.parsing.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-conversation-parsing-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;ai;conversation;parsing")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-conversation-parsing-test)
    endforeach()

    add_executable(umicom-ai-conversation-import-test tests/test_ai_conversation_import.c)
    target_link_libraries(umicom-ai-conversation-import-test PRIVATE Umicom::ai)
    set_target_properties(umicom-ai-conversation-import-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-conversation-import-test)
    umicom_apply_sanitizers(umicom-ai-conversation-import-test)
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-ai-conversation-import-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai/authorengine_service.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai/conversation_archive_fixture.h")
    endif()
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/tests/ai-session-import")
    foreach(_case IN ITEMS insert replace conflict stale missing-replace invalid-record null-record
            null-report null-service source-zero source-max policy capacity last-service service-max
            service-zero read-invalid-path read-zero-budget read-large-budget file-insert file-replace
            file-conflict file-malformed file-limit)
        set(_name "framework.ai.conversation.import.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-conversation-import-test "${_case}"
            "${CMAKE_CURRENT_BINARY_DIR}/tests/ai-session-import/${_case}.session")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;ai;conversation;import")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-conversation-import-test)
    endforeach()
endif()
