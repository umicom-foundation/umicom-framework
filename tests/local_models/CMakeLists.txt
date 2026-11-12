#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/local_models/CMakeLists.txt
#
# PURPOSE:
#   Register offline model-manager and native process ownership regressions without fetching real weights.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

add_executable(umicom-model-plan-test test_plan.c)
add_executable(umicom-model-inspect-test test_inspect.c)
add_executable(umicom-model-stream-test test_stream.c)
add_executable(umicom-model-session-test test_session.c)
add_executable(umicom-model-server-fixture server_fixture.c)
add_dependencies(umicom-model-session-test umicom-model-server-fixture)
foreach(_target IN ITEMS umicom-model-plan-test umicom-model-inspect-test umicom-model-stream-test
        umicom-model-session-test umicom-model-server-fixture)
    target_link_libraries(${_target} PRIVATE Umicom::local_models)
    set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(${_target})
    umicom_apply_sanitizers(${_target})
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${_target})
    endif()
endforeach()
function(umicom_model_case group case target)
    set(_name "framework.local_models.${group}.${case}")
    add_test(NAME "${_name}" COMMAND ${target} "${case}" ${ARGN})
    set_tests_properties("${_name}" PROPERTIES TIMEOUT 20 SKIP_RETURN_CODE 77
        LABELS "framework;local-models;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners("${_name}" TARGETS ${target})
    endif()
endfunction()
foreach(_case IN ITEMS catalogue source-rejection download-admission download-cancelled run-arguments run-limits cli-admission preview)
    umicom_model_case(plan "${_case}" umicom-model-plan-test)
endforeach()
foreach(_case IN ITEMS valid multichunk bad-magic bad-version bad-count truncated wrong-hash cancelled no-overwrite)
    umicom_model_case(inspect "${_case}" umicom-model-inspect-test)
endforeach()
foreach(_case IN ITEMS arguments chunks empty limit cancel fifo symlink)
    umicom_model_case(stream "${_case}" umicom-model-stream-test)
endforeach()
foreach(_case IN ITEMS lifetime wrong-hash)
    umicom_model_case(session "${_case}" umicom-model-session-test "$<TARGET_FILE:umicom-model-server-fixture>")
endforeach()
add_test(NAME framework.local_models.standalone_help COMMAND umicom-models --help)
set_tests_properties(framework.local_models.standalone_help PROPERTIES TIMEOUT 10 LABELS "framework;local-models;cli")
if(TARGET umicom)
    add_test(NAME framework.local_models.unified_help COMMAND umicom models --help)
    set_tests_properties(framework.local_models.unified_help PROPERTIES TIMEOUT 10 LABELS "framework;local-models;cli")
endif()
