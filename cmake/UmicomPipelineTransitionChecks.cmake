#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomPipelineTransitionChecks.cmake
#
# PURPOSE:
#   Register model and runtime revision-boundary regressions using real implementations.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#---------------------------------------------------------------------------
include_guard(GLOBAL)
foreach(_suite IN ITEMS pipeline_transition_plan runtime_pipeline_budget)
    set(_target "umicom-developer-${_suite}-tests")
    add_executable("${_target}" "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_lifecycle/test_${_suite}.c")
    target_link_libraries("${_target}" PRIVATE Umicom::developer)
    set_target_properties("${_target}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings("${_target}")
    umicom_apply_sanitizers("${_target}")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target("${_target}")
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs("${_target}" FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/developer/pipeline.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/developer/runtime.c")
    endif()
endforeach()

function(umicom_pipeline_transition_case name target)
    add_test(NAME "${name}" COMMAND "${target}" ${ARGN})
    set_tests_properties("${name}" PROPERTIES TIMEOUT 30
        LABELS "framework;developer;pipeline;identity;transaction;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners("${name}" TARGETS "${target}")
    endif()
endfunction()
foreach(_action IN ITEMS start start-next progress complete fail cancel retry refresh dependency execution)
    foreach(_budget IN ITEMS pipeline subject descendant boundary unrelated)
        umicom_pipeline_transition_case("framework.developer.pipeline_transition.${_action}.${_budget}"
            umicom-developer-pipeline_transition_plan-tests "${_action}" "${_budget}")
    endforeach()
endforeach()
foreach(_case IN ITEMS no-change attempt-boundary retry-two-changes missing-start empty-next deep deep-refusal)
    umicom_pipeline_transition_case("framework.developer.pipeline_transition.${_case}"
        umicom-developer-pipeline_transition_plan-tests "${_case}")
endforeach()
foreach(_budget IN ITEMS pipeline-zero pipeline-one pipeline-two operation-one descendant refresh-revisit next-start)
    umicom_pipeline_transition_case("framework.developer.pipeline_execution.${_budget}"
        umicom-developer-runtime_pipeline_budget-tests "${_budget}" success)
endforeach()
foreach(_budget IN ITEMS exact unrelated normal)
    foreach(_outcome IN ITEMS success failure cancel timeout error)
        umicom_pipeline_transition_case("framework.developer.pipeline_execution.${_budget}.${_outcome}"
            umicom-developer-runtime_pipeline_budget-tests "${_budget}" "${_outcome}")
    endforeach()
endforeach()
