# Umicom Framework
# File: cmake/UmicomCodingPlatformSnapshot.cmake
#
# PURPOSE:
# Register failure-preserving coding platform observation checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::developer)
    add_executable(umicom-ai-coding-platform-snapshot-test tests/ai_coding_runtime/test_platform_snapshot.c)
    target_link_libraries(umicom-ai-coding-platform-snapshot-test PRIVATE Umicom::developer)
    set_target_properties(umicom-ai-coding-platform-snapshot-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-coding-platform-snapshot-test)
    umicom_apply_sanitizers(umicom-ai-coding-platform-snapshot-test)
    foreach(_stage IN ITEMS agent assistant repository)
        foreach(_status IN ITEMS argument state io capacity)
            set(_name "framework.ai.coding-platform-snapshot.failure.${_stage}.${_status}")
            add_test(NAME "${_name}" COMMAND umicom-ai-coding-platform-snapshot-test failure "${_stage}" "${_status}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;snapshot;ownership")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-platform-snapshot-test)
        endforeach()
    endforeach()
    foreach(_case IN ITEMS idle active counts null-owner null-output)
        set(_name "framework.ai.coding-platform-snapshot.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-platform-snapshot-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;snapshot;ownership")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-platform-snapshot-test)
    endforeach()
endif()
