# Umicom Framework
# File: cmake/UmicomAiEnsembleAdmission.cmake
#
# PURPOSE:
#   Register shared multi-model report regressions and an independent SDK comparison example.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)
set(_umicom_ensemble_root "${CMAKE_CURRENT_LIST_DIR}/..")
if(BUILD_TESTING)
    add_executable(umicom-ai-ensemble-admission-test "${_umicom_ensemble_root}/tests/ai/test_model_ensemble_admission.c")
    target_link_libraries(umicom-ai-ensemble-admission-test PRIVATE Umicom::ai_ui)
    add_executable(umicom-ai-comparison-example-test "${_umicom_ensemble_root}/examples/model_comparison/main.c")
    target_link_libraries(umicom-ai-comparison-example-test PRIVATE Umicom::ai)
    foreach(_target umicom-ai-ensemble-admission-test umicom-ai-comparison-example-test)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
    endforeach()
    foreach(_case empty valid maximum oversized-count max-count selected-outside selected-failed success-count result-size result-version target-size target-version target-enabled target-approved target-provider target-model target-label request-text response-text response-identity response-finish failed-response inactive-slots select-success select-failed select-overflow borrowed-result nulls view-success view-revision-max view-revision-overflow query-target-flags query-request-count query-finish query-partial)
        add_test(NAME framework.ai.ensemble_admission.${_case} COMMAND umicom-ai-ensemble-admission-test ${_case})
        set_tests_properties(framework.ai.ensemble_admission.${_case} PROPERTIES
            TIMEOUT 20 LABELS "framework;ai;comparison;ownership")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.ai.ensemble_admission.${_case} TARGETS umicom-ai-ensemble-admission-test)
        endif()
    endforeach()
    add_test(NAME framework.ai.ensemble_admission.sdk-example COMMAND umicom-ai-comparison-example-test)
    set_tests_properties(framework.ai.ensemble_admission.sdk-example PROPERTIES
        TIMEOUT 20 LABELS "framework;ai;comparison;sdk")
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationBranding.cmake")
    umicom_apply_application_branding(TARGET umicom-ai-comparison-example-test
        PRODUCT_NAME "Umicom Model Comparison Example" INTERNAL_NAME "umicom-ai-comparison-example-test"
        APPLICATION_ID "org.umicom.model-comparison-example"
        RESOURCE_ROOT "${_umicom_ensemble_root}/resources" NO_RUNTIME_DEPLOYMENT)
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(FILES "${_umicom_ensemble_root}/examples/model_comparison/main.c"
        "${_umicom_ensemble_root}/examples/model_comparison/CMakeLists.txt"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/model_comparison"
        COMPONENT "${_component}" ${_selection})
endforeach()
