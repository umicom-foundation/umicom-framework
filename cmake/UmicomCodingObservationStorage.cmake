# Umicom Framework
# File: cmake/UmicomCodingObservationStorage.cmake
#
# PURPOSE:
# Register event and history admission, batch and bounded read qualification.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::developer)
    add_executable(umicom-ai-coding-event-storage-test tests/ai_coding_runtime/test_event_storage.c)
    target_link_libraries(umicom-ai-coding-event-storage-test PRIVATE Umicom::developer)
    set_target_properties(umicom-ai-coding-event-storage-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-coding-event-storage-test)
    umicom_apply_sanitizers(umicom-ai-coding-event-storage-test)
    foreach(_case IN ITEMS null-record empty huge zero null-output null-count null-owner scalar copy capacity full paged wrap)
        set(_name "framework.ai.coding-event-storage.public.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-event-storage-test public "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;observations;storage")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-event-storage-test)
    endforeach()
    foreach(_case IN ITEMS head count stored-text)
        set(_name "framework.ai.coding-event-storage.private.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-event-storage-test private "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;observations;storage")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-event-storage-test)
    endforeach()
    foreach(_mode IN ITEMS unterminated last-byte)
        foreach(_field IN ITEMS task-id message)
            set(_name "framework.ai.coding-event-storage.text.${_mode}.${_field}")
            add_test(NAME "${_name}" COMMAND umicom-ai-coding-event-storage-test text "${_mode}" "${_field}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;observations;storage")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-event-storage-test)
        endforeach()
    endforeach()
    add_executable(umicom-ai-coding-history-storage-test tests/ai_coding_runtime/test_history_storage.c)
    target_link_libraries(umicom-ai-coding-history-storage-test PRIVATE Umicom::developer)
    set_target_properties(umicom-ai-coding-history-storage-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-coding-history-storage-test)
    umicom_apply_sanitizers(umicom-ai-coding-history-storage-test)
    foreach(_case IN ITEMS null-record huge empty zero null-output null-count null-owner past-end maximum-offset end copy scalar range retain replace single-eviction partial-retain)
        set(_name "framework.ai.coding-history-storage.public.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-history-storage-test public "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;observations;storage")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-history-storage-test)
    endforeach()
    foreach(_case IN ITEMS count stored-text)
        set(_name "framework.ai.coding-history-storage.private.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-history-storage-test private "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;observations;storage")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-history-storage-test)
    endforeach()
    foreach(_mode IN ITEMS unterminated last-byte)
        foreach(_field IN ITEMS task-id request-id provider-id model-id patch-id)
            set(_name "framework.ai.coding-history-storage.text.${_mode}.${_field}")
            add_test(NAME "${_name}" COMMAND umicom-ai-coding-history-storage-test text "${_mode}" "${_field}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;observations;storage")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-history-storage-test)
        endforeach()
    endforeach()
endif()
