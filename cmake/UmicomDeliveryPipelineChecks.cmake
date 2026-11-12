#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomDeliveryPipelineChecks.cmake
#
# PURPOSE:
#   Register delivery lifecycle regressions and the installed model consumer.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
if(NOT TARGET Umicom::build)
    return()
endif()
# Ship the same thin consumer taught in the application-delivery lesson.
install(FILES
    "${CMAKE_CURRENT_LIST_DIR}/../examples/delivery_observation/main.c"
    "${CMAKE_CURRENT_LIST_DIR}/../examples/delivery_observation/CMakeLists.txt"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/examples/delivery_observation COMPONENT Learning)
if(NOT BUILD_TESTING)
    return()
endif()
add_executable(umicom-delivery-pipeline-lifecycle-test "${CMAKE_CURRENT_LIST_DIR}/../tests/build/test_pipeline_lifecycle.c")
add_executable(umicom-delivery-observation "${CMAKE_CURRENT_LIST_DIR}/../examples/delivery_observation/main.c")
foreach(target umicom-delivery-pipeline-lifecycle-test umicom-delivery-observation)
    target_link_libraries(${target} PRIVATE Umicom::build)
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(${target})
    umicom_apply_sanitizers(${target})
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${target})
    endif()
endforeach()
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom-delivery-pipeline-lifecycle-test FILES
        "${CMAKE_CURRENT_LIST_DIR}/../src/build/pipeline.c")
endif()
foreach(scenario id-long id-boundary id-empty snapshot-incremental snapshot-tasks snapshot-artifacts snapshot-deployment stop-idle stop-planned stop-task-waiting stop-task-running stop-task-failure stop-task-refusal stop-deployment-waiting stop-deployment-running stop-deployment-failure stop-deployment-refusal artifact-replacement tasks-after-deployment-plan deployment-id-long artifact-id-long target-id-long deployment-id-empty dry-run-invalid plan-generation generation-boundary revision-boundary revision-plan revision-begin-tasks revision-start-task revision-complete-task revision-fail-task revision-stop-task revision-artifact revision-plan-deployment revision-approve-deployment revision-begin-deployment revision-start-step revision-complete-step revision-fail-step revision-stop-deployment)
    set(test framework.build.pipeline_lifecycle.${scenario})
    add_test(NAME ${test} COMMAND umicom-delivery-pipeline-lifecycle-test ${scenario})
    set_tests_properties(${test} PROPERTIES TIMEOUT 30 LABELS "framework;build;pipeline;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(${test} TARGETS umicom-delivery-pipeline-lifecycle-test)
    endif()
endforeach()
foreach(scenario complete cancel fail)
    set(test framework.build.delivery_observation.${scenario})
    add_test(NAME ${test} COMMAND umicom-delivery-observation ${scenario})
    set_tests_properties(${test} PROPERTIES TIMEOUT 30 LABELS "framework;build;pipeline;consumer;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(${test} TARGETS umicom-delivery-observation)
    endif()
endforeach()
