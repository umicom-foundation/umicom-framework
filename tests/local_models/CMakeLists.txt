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

# This fixture is native C and opens only an ephemeral IPv4 loopback listener.
# Consent/argument/cancelled/preview cases do not create a socket at all.
find_package(Threads REQUIRED)
add_executable(umicom-model-health-test test_health.c)
target_link_libraries(umicom-model-health-test PRIVATE Umicom::local_models Threads::Threads)
if(WIN32)
    target_link_libraries(umicom-model-health-test PRIVATE ws2_32)
else()
    target_compile_definitions(umicom-model-health-test PRIVATE _POSIX_C_SOURCE=200809L)
endif()
set_target_properties(umicom-model-health-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umicom-model-health-test)
umicom_apply_sanitizers(umicom-model-health-test)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-model-health-test)
endif()
foreach(_case IN ITEMS consent arguments cancelled preview ready loading proxy-bypass
        bad-json oversized redirect unauthorized timeout cancel)
    umicom_model_case(health "${_case}" umicom-model-health-test)
endforeach()

# The pure readiness cases do not need an HTTP implementation. The unavailable
# case deliberately requires a separate HTTP-OFF build and reports a skip here.
add_executable(umicom-model-readiness-test test_readiness.c)
target_link_libraries(umicom-model-readiness-test PRIVATE Umicom::local_models)
set_target_properties(umicom-model-readiness-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umicom-model-readiness-test)
umicom_apply_sanitizers(umicom-model-readiness-test)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-model-readiness-test)
endif()
foreach(_case IN ITEMS budget one boundary attempts-max port attempts-zero attempts-over
        pause-low pause-high timeout-low timeout-high budget-over wide null consent
        cancelled preview cli-rejection unavailable)
    umicom_model_case(readiness "${_case}" umicom-model-readiness-test)
endforeach()
foreach(_case IN ITEMS wait-ready wait-loading-ready wait-loading-limit wait-disconnect-ready
        wait-disconnect-limit wait-bad-json wait-oversized wait-redirect wait-unauthorized
        wait-cancel wait-cancel-after-response wait-stale wait-cli-ready wait-cli-limit wait-cli-preview)
    umicom_model_case(readiness_http "${_case}" umicom-model-health-test)
endforeach()

# Runtime descriptions are pure; output failure hooks prevent side effects.
foreach(_suite IN ITEMS runtime cli-review session-metadata)
    if(_suite STREQUAL "runtime")
        set(_source test_runtime.c)
    elseif(_suite STREQUAL "session-metadata")
        set(_source test_session_metadata.c)
    else()
        set(_source test_cli_review.c)
    endif()
    set(_target "umicom-model-${_suite}-test")
    add_executable("${_target}" "${_source}")
    target_link_libraries("${_target}" PRIVATE Umicom::local_models)
    set_target_properties("${_target}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings("${_target}")
    umicom_apply_sanitizers("${_target}")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target("${_target}")
    endif()
endforeach()
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom-model-session-metadata-test FILES
        "${CMAKE_CURRENT_LIST_DIR}/../../src/local_models/session.c")
    umicom_target_qualification_inputs(umicom-model-cli-review-test FILES
        "${CMAKE_CURRENT_LIST_DIR}/../../src/local_models/cli.c")
endif()
foreach(_case IN ITEMS metadata null id id-empty id-space label label-empty output-zero
        output-large output-context port context threads alias digest guard directory executable model)
    umicom_model_case(runtime "${_case}" umicom-model-runtime-test)
endforeach()
foreach(_case IN ITEMS describe describe-apply describe-invalid flush stream)
    umicom_model_case(cli_review "${_case}" umicom-model-cli-review-test)
endforeach()

foreach(_case IN ITEMS partial copy)
    umicom_model_case(session_metadata "${_case}" umicom-model-session-metadata-test)
endforeach()

# Retained-owner tests use inert native boundaries, never a real child process.
add_executable(umicom-model-console-owner-test test_console_owner.c)
target_link_libraries(umicom-model-console-owner-test PRIVATE Umicom::local_models)
set_target_properties(umicom-model-console-owner-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umicom-model-console-owner-test)
umicom_apply_sanitizers(umicom-model-console-owner-test)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-model-console-owner-test)
endif()
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom-model-console-owner-test FILES
        "${CMAKE_CURRENT_LIST_DIR}/../../src/local_models/cli.c")
endif()
foreach(_case IN ITEMS null occupied preview legacy-retained legacy-consumed start-unowned start-clean
        start-retained start-consumed stop-retained stop-consumed output-clean output-retained success)
    umicom_model_case(console_owner "${_case}" umicom-model-console-owner-test)
endforeach()

# Owner-run configuration fixtures use the actual independent consumer and
# production admission helpers. Placeholder archives cannot qualify linking.
foreach(_case IN ITEMS explicit discovered spaces cached-other switch-root relative missing-root
        missing-component missing-guard wrong-prefix changed-header extra-header
        outside-library missing-archive abi compiler metadata)
    set(_test "framework.local_models.sdk_selection.${_case}")
    add_test(NAME "${_test}" COMMAND "${CMAKE_COMMAND}"
        "-DFRAMEWORK=${CMAKE_CURRENT_LIST_DIR}/../.."
        "-DOUTPUT_ROOT=${CMAKE_CURRENT_BINARY_DIR}/sdk-selection/${_case}"
        "-DGENERATOR=${CMAKE_GENERATOR}"
        "-DC_COMPILER=${CMAKE_C_COMPILER}"
        "-DMAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}"
        "-DTOOLCHAIN=${CMAKE_TOOLCHAIN_FILE}"
        "-DCASE=${_case}"
        -P "${CMAKE_CURRENT_LIST_DIR}/sdk_selection/check.cmake")
    set_tests_properties("${_test}" PROPERTIES TIMEOUT 240
        LABELS "framework;local-models;sdk;configuration;regression")
endforeach()

# Host correlation fixtures use inert owners; identity contention uses native
# Framework threads without launching a model or accessing the network.
add_executable(umicom-model-request-binding-test test_request_binding.c)
target_link_libraries(umicom-model-request-binding-test PRIVATE Umicom::local_models)
set_target_properties(umicom-model-request-binding-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umicom-model-request-binding-test)
umicom_apply_sanitizers(umicom-model-request-binding-test)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-model-request-binding-test)
endif()
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom-model-request-binding-test FILES
        "${CMAKE_CURRENT_LIST_DIR}/../../src/local_models/session.c")
endif()
foreach(_case IN ITEMS valid copy alias-input null-session null-output null-request empty-request invalid-request unterminated-request maximum-request partial-supervisor partial-job zero-identity invalid-plan)
    set(_test "framework.local_models.request_binding.bind.${_case}")
    add_test(NAME "${_test}" COMMAND umicom-model-request-binding-test bind "${_case}")
    set_tests_properties("${_test}" PROPERTIES TIMEOUT 20 LABELS "framework;local-models;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners("${_test}" TARGETS umicom-model-request-binding-test)
    endif()
endforeach()
foreach(_case IN ITEMS match identity request digest alias port context threads null-session null-binding null-request empty-request invalid-request)
    set(_test "framework.local_models.request_binding.check.${_case}")
    add_test(NAME "${_test}" COMMAND umicom-model-request-binding-test check "${_case}")
    set_tests_properties("${_test}" PROPERTIES TIMEOUT 20 LABELS "framework;local-models;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners("${_test}" TARGETS umicom-model-request-binding-test)
    endif()
endforeach()
foreach(_case IN ITEMS request alias digest identity port context-low context-high threads-low threads-high alias-option)
    set(_test "framework.local_models.request_binding.malformed.${_case}")
    add_test(NAME "${_test}" COMMAND umicom-model-request-binding-test malformed "${_case}")
    set_tests_properties("${_test}" PROPERTIES TIMEOUT 20 LABELS "framework;local-models;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners("${_test}" TARGETS umicom-model-request-binding-test)
    endif()
endforeach()
foreach(_case IN ITEMS reserve exhaustion concurrent)
    set(_test "framework.local_models.request_binding.identity.${_case}")
    add_test(NAME "${_test}" COMMAND umicom-model-request-binding-test identity "${_case}")
    set_tests_properties("${_test}" PROPERTIES TIMEOUT 20 LABELS "framework;local-models;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners("${_test}" TARGETS umicom-model-request-binding-test)
    endif()
endforeach()
