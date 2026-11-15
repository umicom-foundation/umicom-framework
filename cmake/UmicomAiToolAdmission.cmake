# Umicom Framework
# File: cmake/UmicomAiToolAdmission.cmake
#
# PURPOSE:
# Register native tool descriptor, registry and policy admission regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::ai)
    add_executable(umicom-ai-tool-admission-test tests/ai/test_tool_admission.c)
    target_link_libraries(umicom-ai-tool-admission-test PRIVATE Umicom::ai)
    set_target_properties(umicom-ai-tool-admission-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-tool-admission-test)
    umicom_apply_sanitizers(umicom-ai-tool-admission-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ai-tool-admission-test)
    endif()
    function(_umicom_tool_admission_case mode name)
        set(_name "framework.ai.tool-admission.${mode}.${name}")
        add_test(NAME "${_name}" COMMAND umicom-ai-tool-admission-test "${mode}" "${name}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;tool;admission")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-tool-admission-test)
    endfunction()
    foreach(_mode IN ITEMS descriptor boundary construct-boundary construct-overflow)
        foreach(_field IN ITEMS id description permission)
            _umicom_tool_admission_case("${_mode}" "${_field}")
        endforeach()
    endforeach()
    foreach(_case IN ITEMS count huge-count duplicate late-record callback empty-id id description permission)
        _umicom_tool_admission_case(registry "${_case}")
    endforeach()
    foreach(_case IN ITEMS alias empty-id null-callback opaque-metadata inactive copied missing empty-query long-query nulls)
        _umicom_tool_admission_case(misc "${_case}")
    endforeach()
    foreach(_case IN ITEMS policy approval missing success callback-error)
        _umicom_tool_admission_case(dispatch "${_case}")
    endforeach()
    _umicom_tool_admission_case(capacity full)
endif()
