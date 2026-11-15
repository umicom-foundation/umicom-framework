# Umicom Framework
# File: cmake/UmicomCodingRuntimeLifetime.cmake
#
# PURPOSE:
# Register native coding execution and parent callback ownership regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::developer)
    add_executable(umicom-ai-coding-runtime-lifetime-test tests/ai_coding_runtime/test_owner_lifetime.c)
    target_link_libraries(umicom-ai-coding-runtime-lifetime-test PRIVATE Umicom::developer)
    set_target_properties(umicom-ai-coding-runtime-lifetime-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-coding-runtime-lifetime-test)
    umicom_apply_sanitizers(umicom-ai-coding-runtime-lifetime-test)
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-ai-coding-runtime-lifetime-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/platform.c")
    endif()
    set(_auto_phases read provider write execute)
    set(_direct_phases read provider write execute)
    set(_queued_phases read provider write execute)
    set(_manual_phases read write execute)
    set(_repair_phases read provider write execute)
    set(_assistant_phases read write)
    foreach(_entry IN ITEMS auto direct queued manual repair assistant)
        foreach(_phase IN LISTS _${_entry}_phases)
            set(_name "framework.ai.coding-runtime-lifetime.execution.${_entry}.${_phase}")
            add_test(NAME "${_name}" COMMAND umicom-ai-coding-runtime-lifetime-test execution "${_entry}" "${_phase}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 60 LABELS "framework;ai;coding-runtime;lifetime")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-runtime-lifetime-test)
        endforeach()
    endforeach()
    foreach(_phase IN ITEMS read provider write execute)
        set(_name "framework.ai.coding-runtime-lifetime.failure.${_phase}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-runtime-lifetime-test failure "${_phase}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 60 LABELS "framework;ai;coding-runtime;lifetime")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-runtime-lifetime-test)
    endforeach()
    foreach(_case IN ITEMS cancel null selection)
        set(_name "framework.ai.coding-runtime-lifetime.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-runtime-lifetime-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 60 LABELS "framework;ai;coding-runtime;lifetime")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-runtime-lifetime-test)
    endforeach()
endif()
