#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiRuntimeCatalogueChecks.cmake
#
# PURPOSE:
# Register bounded runtime metadata and stale health publication regressions.
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
        set(_target "umicom-ai-catalogue-${_suite}-test")
        add_executable("${_target}" "tests/test_ai_runtime_catalogue_${_suite}.c")
        target_link_libraries("${_target}" PRIVATE Umicom::ai)
        set_target_properties("${_target}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings("${_target}")
        umicom_apply_sanitizers("${_target}")
    endforeach()
    add_executable(umicom-ai-health-revision-test tests/test_ai_authorengine_health_revision.c)
    target_link_libraries(umicom-ai-health-revision-test PRIVATE Umicom::ai)
    set_target_properties(umicom-ai-health-revision-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-health-revision-test)
    umicom_apply_sanitizers(umicom-ai-health-revision-test)
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-ai-catalogue-revision-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai/runtime_catalogue.c")
    endif()
    foreach(_case IN ITEMS runtime provider display model endpoint health-provider health-message
            empty-id empty-provider empty-display empty-model provider-kind transport context
            output-zero output-large health-identity extension-flags last-terminator lookup)
        set(_name "framework.ai.catalogue.admission.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-catalogue-admission-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;ai;catalogue;admission")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-catalogue-admission-test)
    endforeach()
    foreach(_case IN ITEMS conditional provider-replaced health-input saturation capacity count
            zero-revision record-revision zero-record-revision stored-text duplicate)
        set(_name "framework.ai.catalogue.revision.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-catalogue-revision-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;ai;catalogue;revision")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-catalogue-revision-test)
    endforeach()
    foreach(_case IN ITEMS success replace append provider-mismatch malformed-health probe-failure)
        set(_name "framework.ai.catalogue.health.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-health-revision-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;ai;catalogue;health")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-health-revision-test)
    endforeach()

    add_executable(umicom-ai-runtime-health-test tests/test_ai_runtime_health_routing.c)
    target_link_libraries(umicom-ai-runtime-health-test PRIVATE Umicom::ai)
    set_target_properties(umicom-ai-runtime-health-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-runtime-health-test)
    umicom_apply_sanitizers(umicom-ai-runtime-health-test)
    foreach(_case IN ITEMS null-service null-callback null-output empty routes same-model
            unconfigured unavailable timeout cancel malformed provider-mismatch replace append
            reenter-runtime reenter-legacy legacy-reenter partial null-context)
        set(_name "framework.ai.catalogue.runtime_health.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-runtime-health-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;ai;catalogue;health")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-runtime-health-test)
    endforeach()

    add_executable(umicom-ai-service-revision-test tests/test_ai_authorengine_service_revision.c)
    target_link_libraries(umicom-ai-service-revision-test PRIVATE Umicom::ai)
    set_target_properties(umicom-ai-service-revision-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-service-revision-test)
    umicom_apply_sanitizers(umicom-ai-service-revision-test)
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-ai-service-revision-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai/authorengine_service.c")
    endif()
    foreach(_case IN ITEMS partial complete empty first-failure partial-legacy health-max
            health-partial-max health-callback-max policy-max runtime-max session-max message-max
            load-max zero-revision message-record-max message-record-zero)
        set(_name "framework.ai.catalogue.service_revision.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-service-revision-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;ai;catalogue;revision")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-service-revision-test)
    endforeach()
endif()
