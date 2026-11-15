# Umicom Framework
# File: cmake/UmicomAiProviderInvocationLifetime.cmake
#
# PURPOSE:
# Register synchronous provider callback ownership and teardown checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::developer)
    add_executable(umicom-ai-provider-invocation-test tests/ai/test_provider_invocation_lifetime.c)
    target_link_libraries(umicom-ai-provider-invocation-test PRIVATE Umicom::developer)
    set_target_properties(umicom-ai-provider-invocation-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-provider-invocation-test)
    umicom_apply_sanitizers(umicom-ai-provider-invocation-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ai-provider-invocation-test)
    endif()
    foreach(_entry IN ITEMS checked legacy)
        foreach(_action IN ITEMS registry-idle runtime-idle registry-release runtime-release
            registry-destroy runtime-destroy registry-add registry-init runtime-init
            legacy-recursion checked-recursion nested-other)
            foreach(_outcome IN ITEMS success provider-error invalid-response)
                set(_name "framework.ai.provider-invocation.${_entry}.${_action}.${_outcome}")
                add_test(NAME "${_name}" COMMAND umicom-ai-provider-invocation-test "${_entry}" "${_action}" "${_outcome}")
                set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;lifetime;callback")
                umicom_test_impact_owners("${_name}" TARGETS umicom-ai-provider-invocation-test)
            endforeach()
        endforeach()
    endforeach()
    add_test(NAME framework.ai.provider-invocation.nulls COMMAND umicom-ai-provider-invocation-test nulls)
    set_tests_properties(framework.ai.provider-invocation.nulls PROPERTIES TIMEOUT 45 LABELS "framework;ai;lifetime;callback")
    umicom_test_impact_owners(framework.ai.provider-invocation.nulls TARGETS umicom-ai-provider-invocation-test)
endif()
