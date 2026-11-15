# Umicom Framework
# File: cmake/UmicomAiCheckedGeneration.cmake
#
# PURPOSE:
# Register bounded provider value and checked response publication regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::developer)
    add_executable(umicom-ai-checked-generation-test tests/ai/test_checked_generation.c)
    target_link_libraries(umicom-ai-checked-generation-test PRIVATE Umicom::developer)
    set_target_properties(umicom-ai-checked-generation-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-checked-generation-test)
    umicom_apply_sanitizers(umicom-ai-checked-generation-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ai-checked-generation-test)
    endif()
    function(_umicom_generation_case mode name)
        set(_name "framework.ai.checked-generation.${mode}.${name}")
        add_test(NAME "${_name}" COMMAND umicom-ai-checked-generation-test "${mode}" "${name}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;admission;ownership")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-checked-generation-test)
    endfunction()
    foreach(_field IN ITEMS request-id model-id message-name message-text)
        _umicom_generation_case(request "${_field}")
        _umicom_generation_case(request-boundary "${_field}")
    endforeach()
    foreach(_case IN ITEMS count count-max nan infinity role-low role-high unused draft full last-slot finite-provider-range)
        _umicom_generation_case(request "${_case}")
    endforeach()
    foreach(_field IN ITEMS request-id provider-id model-id text)
        _umicom_generation_case(response "${_field}")
        _umicom_generation_case(response-boundary "${_field}")
    endforeach()
    foreach(_case IN ITEMS finish-low finish-high known-finishes usage-evidence)
        _umicom_generation_case(response "${_case}")
    endforeach()
    foreach(_entry IN ITEMS checked legacy coding)
        foreach(_case IN ITEMS bad-request missing-provider policy registry provider-error invalid-response invalid-finish success selection-change)
            set(_name "framework.ai.checked-generation.dispatch.${_entry}.${_case}")
            add_test(NAME "${_name}" COMMAND umicom-ai-checked-generation-test dispatch "${_entry}" "${_case}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;admission;ownership")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-checked-generation-test)
        endforeach()
    endforeach()
    foreach(_case IN ITEMS provider-id model-id temperature kind budget)
        _umicom_generation_case(config "${_case}")
    endforeach()
    foreach(_case IN ITEMS runtime id request response null-report value-nulls)
        _umicom_generation_case(optional "${_case}")
    endforeach()
endif()
