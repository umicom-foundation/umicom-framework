# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Each case owns a separate fixture folder; parallel tests never share a launch or log file.
include_guard(GLOBAL)
if(NOT BUILD_TESTING)
    return()
endif()
function(umicom_run_current_target target source)
    add_executable("${target}" "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/run_current/${source}")
    target_link_libraries("${target}" PRIVATE Umicom::build)
    set_target_properties("${target}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings("${target}")
    umicom_apply_sanitizers("${target}")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target("${target}")
    endif()
endfunction()
function(umicom_run_current_case group case target)
    set(directory "${CMAKE_CURRENT_BINARY_DIR}/run-current-fixtures/${group}-${case}")
    file(MAKE_DIRECTORY "${directory}")
    add_test(NAME framework.run_current.${group}.${case} COMMAND "${target}" "${case}")
    set_tests_properties(framework.run_current.${group}.${case} PROPERTIES
        WORKING_DIRECTORY "${directory}" TIMEOUT 30 LABELS "framework;build;launch;regression")
endfunction()
umicom_run_current_target(umicom-run-current-prepare-test test_prepare.c)
umicom_run_current_target(umicom-run-current-session-test test_session.c)
foreach(case relative alias bare missing directory working-folder empty environment)
    umicom_run_current_case(prepare "${case}" umicom-run-current-prepare-test)
endforeach()
foreach(case copied cancel failure log log-existing repeat)
    umicom_run_current_case(session "${case}" umicom-run-current-session-test)
endforeach()
if(TARGET umicom-environment-producer)
    umicom_run_current_target(umicom-run-current-native-test test_native.c)
    add_dependencies(umicom-run-current-native-test umicom-environment-producer)
    set(directory "${CMAKE_CURRENT_BINARY_DIR}/run-current-fixtures/native")
    file(MAKE_DIRECTORY "${directory}")
    add_test(NAME framework.run_current.native COMMAND umicom-run-current-native-test "$<TARGET_FILE:umicom-environment-producer>")
    set_tests_properties(framework.run_current.native PROPERTIES
        WORKING_DIRECTORY "${directory}" TIMEOUT 30 LABELS "framework;build;launch;native")
endif()

# Review shares launch preparation without submitting a job or executing the fixture.
umicom_run_current_target(umicom-run-current-review-test test_review.c)
foreach(case ready empty missing directory working-folder source-folder bare environment arguments buffer)
    umicom_run_current_case(review "${case}" umicom-run-current-review-test)
endforeach()

# Restart uses the same controlled executor fixtures; no test compiles a project.
umicom_run_current_target(umicom-run-restart-test test_restart.c)
foreach(case invalid idle configure build test run-configure run-build untrusted other-project changed-profile
    success waiting handover duplicate pending-submit destroy stop thread-stop revoked changed-after other-project-after
    bad-count missing-program log log-collision)
    umicom_run_current_case(restart "${case}" umicom-run-restart-test)
endforeach()
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom-run-restart-test FILES
        "${CMAKE_CURRENT_LIST_DIR}/../src/build/run_restart.inc"
        "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/build/run_restart.h")
endif()
