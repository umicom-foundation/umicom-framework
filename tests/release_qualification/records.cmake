# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Fixtures use injected observations and outcome records; no tool execution.
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomImpactInventory.cmake")
foreach(_group IN ITEMS identity recording assessment scale inventory_identity report batch input_coverage view interruption archive_parse archive_file archive_semantics compare)
    set(_target "umi-qualification-${_group}-test")
    add_executable("${_target}"
        "${CMAKE_CURRENT_LIST_DIR}/test_${_group}.c"
        "${CMAKE_CURRENT_LIST_DIR}/fixture.c"
        "${CMAKE_CURRENT_LIST_DIR}/../release_impact/fixture.c")
    target_link_libraries("${_target}" PRIVATE Umicom::distribution)
    umicom_target_qualification_inputs("${_target}" FILES
        "${CMAKE_CURRENT_LIST_DIR}/fixture.h" "${CMAKE_CURRENT_LIST_DIR}/../release_impact/fixture.h")
    if(_group STREQUAL "archive_file")
        umicom_target_qualification_inputs("${_target}" FILES
            "${CMAKE_CURRENT_LIST_DIR}/../build_log/fixture.h"
            "${CMAKE_CURRENT_LIST_DIR}/../support/symbolic_link.h")
    endif()
    set_target_properties("${_target}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings("${_target}")
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers("${_target}")
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target("${_target}")
    endif()
endforeach()
set(_qualification_identity_cases ownership attempt toolchain context invalid source selection no-work)
set(_qualification_recording_cases build-first invalid-result duplicate-build unknown-test duplicate-test copy failed-build)
set(_qualification_assessment_cases passed missing-build missing-test no-tests failed skipped not-run cancelled timed-out
    source-changed context-changed unobserved-source toolchain-changed unobserved-toolchain invalid-close sealed)
set(_qualification_scale_cases many-tests long-name)
set(_qualification_report_cases missing passed skipped framing many-rows capacity invalid deterministic)
set(_qualification_batch_cases order unknown-last duplicate invalid-last existing empty unknown-exit process process-failed partial-process unknown-process-exit disabled)
set(_qualification_input_coverage_cases header resource shared unresolved)
set(_qualification_view_cases invalid large pagination problems ownership passed source-blocked missing)
set(_qualification_interruption_cases before-build after-pass)
set(_qualification_archive_parse_cases passed open no-tests process interrupted unobserved large version identity
    counts ready numeric-overflow oversized-count oversized-name leading-zero invalid-mask source-pair open-after unknown-outcome invalid-pass
    unknown-exit invalid-name duplicate unsorted trailing unknown-row truncated embedded-nul invalid-arguments)
set(_qualification_archive_file_cases reopen receipt wrong-digest existing malformed missing relative open)
set(_qualification_archive_semantics_cases mixed names partial failed-build unknown-process integer-boundaries changed-tools)
set(_qualification_compare_cases same regressed recovered changed added removed missing source-changed log-duration saved open invalid large)
foreach(_group IN ITEMS identity recording assessment scale report batch input_coverage view interruption archive_parse archive_file archive_semantics compare)
    foreach(_case IN LISTS _qualification_${_group}_cases)
        set(_name "framework.qualification.${_group}.${_case}")
        add_test(NAME "${_name}" COMMAND "umi-qualification-${_group}-test" "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 60 LABELS "framework;qualification;regression")
        umicom_test_impact_owners("${_name}" TARGETS "umi-qualification-${_group}-test")
    endforeach()
endforeach()
add_test(NAME framework.qualification.inventory_identity COMMAND umi-qualification-inventory_identity-test)
set_tests_properties(framework.qualification.inventory_identity PROPERTIES TIMEOUT 30 LABELS "framework;qualification;regression")
umicom_test_impact_owners(framework.qualification.inventory_identity TARGETS umi-qualification-inventory_identity-test)

# Additional inputs affect coverage without entering compiler SOURCES.
# Retain the former case list; the added generated-only input covers metadata
# that is intentionally absent from the compiler source list.
#[==[
foreach(_case IN ITEMS basic duplicate runtime cross-directory shared missing generated
        bad-owner alias imported expression directory no-files unknown-argument)
]==]
foreach(_case IN ITEMS basic duplicate runtime cross-directory shared missing generated generated-declared-only
        bad-owner alias imported expression directory no-files unknown-argument)
    set(_name "framework.qualification.inputs.${_case}")
    add_test(NAME "${_name}" COMMAND "${CMAKE_COMMAND}"
        "-DROOT=${CMAKE_CURRENT_LIST_DIR}/../.." "-DCASE=${_case}"
        "-DDEST=${CMAKE_CURRENT_BINARY_DIR}/qualification-inputs-${_case}"
        -P "${CMAKE_CURRENT_LIST_DIR}/check_inputs.cmake")
    set_tests_properties("${_name}" PROPERTIES TIMEOUT 60 LABELS "framework;qualification;cmake;regression")
    umicom_test_impact_owners("${_name}" ALWAYS)
endforeach()
