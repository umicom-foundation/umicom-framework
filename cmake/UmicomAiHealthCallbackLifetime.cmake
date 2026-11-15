# Umicom Framework
# File: cmake/UmicomAiHealthCallbackLifetime.cmake
#
# PURPOSE:
# Register health service, borrowed runtime and callback teardown regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::developer)
    add_executable(umicom-ai-health-lifetime-test tests/ai/test_health_callback_lifetime.c)
    target_link_libraries(umicom-ai-health-lifetime-test PRIVATE Umicom::developer)
    set_target_properties(umicom-ai-health-lifetime-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-health-lifetime-test)
    umicom_apply_sanitizers(umicom-ai-health-lifetime-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ai-health-lifetime-test)
    endif()
    foreach(_entry IN ITEMS legacy scoped)
        foreach(_action IN ITEMS service-idle service-destroy service-legacy runtime-idle
            runtime-release runtime-legacy registry-add generate other-service)
            foreach(_outcome IN ITEMS success provider-error malformed)
                set(_name "framework.ai.health-lifetime.${_entry}.${_action}.${_outcome}")
                add_test(NAME "${_name}" COMMAND umicom-ai-health-lifetime-test "${_entry}" "${_action}" "${_outcome}")
                set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;lifetime;health")
                umicom_test_impact_owners("${_name}" TARGETS umicom-ai-health-lifetime-test)
            endforeach()
        endforeach()
        foreach(_action IN ITEMS registry-count health-from-generation)
            set(_name "framework.ai.health-lifetime.admission.${_entry}.${_action}")
            add_test(NAME "${_name}" COMMAND umicom-ai-health-lifetime-test "${_entry}" "${_action}" success)
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;lifetime;health")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-health-lifetime-test)
        endforeach()
    endforeach()
    add_test(NAME framework.ai.health-lifetime.nulls COMMAND umicom-ai-health-lifetime-test nulls)
    set_tests_properties(framework.ai.health-lifetime.nulls PROPERTIES TIMEOUT 45 LABELS "framework;ai;lifetime;health")
    umicom_test_impact_owners(framework.ai.health-lifetime.nulls TARGETS umicom-ai-health-lifetime-test)
endif()
