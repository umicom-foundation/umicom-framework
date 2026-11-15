# Umicom Framework
# File: cmake/UmicomCodingSelectionSpans.cmake
#
# PURPOSE:
# Register bounded source selection and overlap compatibility checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::developer)
    add_executable(umicom-ai-coding-selection-spans-test tests/ai_coding_runtime/test_selection_spans.c)
    target_link_libraries(umicom-ai-coding-selection-spans-test PRIVATE Umicom::developer)
    set_target_properties(umicom-ai-coding-selection-spans-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-coding-selection-spans-test)
    umicom_apply_sanitizers(umicom-ai-coding-selection-spans-test)
    foreach(_case IN ITEMS first middle multiple to-eof beyond-eof empty empty-later trailing-empty crlf cr-only maximum-end unicode unterminated-span null-empty-span)
        set(_name "framework.ai.coding-selection-span.range.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-selection-spans-test range "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;selection;spans")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-selection-spans-test)
    endforeach()
    foreach(_case IN ITEMS null-span size-max embedded-nul zero-line reverse zero-capacity null-output null-length legacy-null)
        set(_name "framework.ai.coding-selection-span.invalid.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-selection-spans-test invalid "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;selection;spans")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-selection-spans-test)
    endforeach()
    foreach(_case IN ITEMS exact short one)
        set(_name "framework.ai.coding-selection-span.capacity.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-selection-spans-test capacity "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;selection;spans")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-selection-spans-test)
    endforeach()
    foreach(_api IN ITEMS span legacy)
        foreach(_direction IN ITEMS same forward backward)
            set(_name "framework.ai.coding-selection-span.alias.${_api}.${_direction}")
            add_test(NAME "${_name}" COMMAND umicom-ai-coding-selection-spans-test alias "${_api}" "${_direction}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;selection;spans")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-selection-spans-test)
        endforeach()
    endforeach()
endif()
