# Umicom Framework
# File: cmake/UmicomCodingPromptUnicode.cmake
#
# PURPOSE:
# Register UTF-8 prompt boundary, malformed scalar and metadata checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::developer)
    add_executable(umicom-ai-coding-prompt-unicode-test tests/ai_coding_runtime/test_prompt_unicode.c)
    target_link_libraries(umicom-ai-coding-prompt-unicode-test PRIVATE Umicom::developer)
    set_target_properties(umicom-ai-coding-prompt-unicode-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-coding-prompt-unicode-test)
    umicom_apply_sanitizers(umicom-ai-coding-prompt-unicode-test)
    foreach(_field IN ITEMS source feedback)
        foreach(_width IN ITEMS 2 3 4)
            foreach(_overlap RANGE 0 ${_width})
                set(_name "framework.ai.coding-prompt-unicode.boundary.${_field}.${_width}.${_overlap}")
                add_test(NAME "${_name}" COMMAND umicom-ai-coding-prompt-unicode-test boundary "${_field}" "${_width}" "${_overlap}")
                set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;prompt;utf8")
                umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-prompt-unicode-test)
            endforeach()
        endforeach()
        foreach(_case IN ITEMS continuation overlong-two overlong-three surrogate above-unicode truncated bad-continuation late)
            set(_name "framework.ai.coding-prompt-unicode.invalid.${_field}.${_case}")
            add_test(NAME "${_name}" COMMAND umicom-ai-coding-prompt-unicode-test invalid "${_field}" "${_case}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;prompt;utf8")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-prompt-unicode-test)
        endforeach()
    endforeach()
    foreach(_field IN ITEMS request-id model-id instruction path)
        set(_name "framework.ai.coding-prompt-unicode.metadata.${_field}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-prompt-unicode-test metadata "${_field}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;prompt;utf8")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-prompt-unicode-test)
    endforeach()
    add_test(NAME framework.ai.coding-prompt-unicode.roundtrip COMMAND umicom-ai-coding-prompt-unicode-test roundtrip)
    set_tests_properties(framework.ai.coding-prompt-unicode.roundtrip PROPERTIES TIMEOUT 45 LABELS "framework;ai;prompt;utf8")
    umicom_test_impact_owners(framework.ai.coding-prompt-unicode.roundtrip TARGETS umicom-ai-coding-prompt-unicode-test)
endif()
