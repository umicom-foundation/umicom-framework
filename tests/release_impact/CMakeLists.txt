#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/release_impact/CMakeLists.txt
# Purpose: Register focused impact-planning regression cases and prerequisites.
# Author and Organisation: Sammy Hegab, Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomImpactInventory.cmake")
foreach(_group IN ITEMS graph fallback paths metadata expression scale relationships required_tests)
    add_executable("umicom-impact-${_group}-test"
        "${CMAKE_CURRENT_LIST_DIR}/test_${_group}.c" "${CMAKE_CURRENT_LIST_DIR}/fixture.c")
    target_link_libraries("umicom-impact-${_group}-test" PRIVATE Umicom::distribution)
    set_target_properties("umicom-impact-${_group}-test" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings("umicom-impact-${_group}-test")
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers("umicom-impact-${_group}-test")
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target("umicom-impact-${_group}-test")
    endif()
endforeach()
set(_impact_required_tests_cases single duplicate related reverse self always source union unknown case-sensitive empty full no-tests incomplete malformed unicode literal null-list null-name empty-name count-limit name-limit total-limit null-inventory null-output occupied-output ownership legacy scale)
set(_impact_relationships_cases chain reverse cycle overlap duplicate self missing unowned always no-changes full legacy-key wrong-kind scale)
set(_impact_graph_cases chain reverse cycle shared-source prerequisite always)
set(_impact_fallback_cases old-inventory unknown-path configuration unowned-test unresolved-source missing-dependency explicit-full no-changes no-tests)
set(_impact_paths_cases rename duplicate boundary header separator invalid)
set(_impact_expression_cases exact full empty control capacity large)
set(_impact_metadata_cases flag hex unknown-key truncated duplicate-flag wrong-kind)
foreach(_group IN ITEMS graph fallback paths metadata expression relationships required_tests)
    foreach(_case IN LISTS _impact_${_group}_cases)
        set(_name "framework.build_impact.${_group}.${_case}")
        add_test(NAME "${_name}" COMMAND "umicom-impact-${_group}-test" "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;build-impact;regression")
        umicom_test_impact_owners("${_name}" TARGETS "umicom-impact-${_group}-test")
    endforeach()
endforeach()
add_test(NAME framework.build_impact.scale COMMAND umicom-impact-scale-test)
set_tests_properties(framework.build_impact.scale PROPERTIES TIMEOUT 60 LABELS "framework;build-impact;regression")
umicom_test_impact_owners(framework.build_impact.scale TARGETS umicom-impact-scale-test)

foreach(_case IN ITEMS basic expression link-only imported-interface test-dependency fixture-required fixture-setup fixture-cleanup bad-owner bad-test)
    set(_name "framework.build_impact.cmake.${_case}")
    add_test(NAME "${_name}" COMMAND "${CMAKE_COMMAND}"
        "-DROOT=${CMAKE_CURRENT_LIST_DIR}/../.."
        "-DDEST=${CMAKE_CURRENT_BINARY_DIR}/impact-cmake-${_case}"
        "-DCASE=${_case}" -P "${CMAKE_CURRENT_LIST_DIR}/check_metadata.cmake")
    set_tests_properties("${_name}" PROPERTIES TIMEOUT 60 LABELS "framework;build-impact;cmake;regression")
    umicom_test_impact_owners("${_name}" ALWAYS)
endforeach()

# Cross-directory test properties are available from CMake 3.28. Older producers
# retain the existing broad fallback; do not claim this feature was exercised.
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.28")
    foreach(_case IN ITEMS dependency fixture multiple-setup cleanup-only overlap
            missing-dependency missing-fixture expression self-fixture cross-directory
            duplicate-test false-name reindex unowned fixture-scale)
        set(_name "framework.build_impact.test_graph.${_case}")
        add_test(NAME "${_name}" COMMAND "${CMAKE_COMMAND}"
            "-DROOT=${CMAKE_CURRENT_LIST_DIR}/../.."
            "-DDEST=${CMAKE_CURRENT_BINARY_DIR}/impact-test-graph-${_case}"
            "-DCASE=${_case}" -P "${CMAKE_CURRENT_LIST_DIR}/check_test_graph.cmake")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 120
            LABELS "framework;build-impact;cmake;regression")
        umicom_test_impact_owners("${_name}" ALWAYS)
    endforeach()
endif()

# Observe source bytes through an injected reader in pure service regressions.
# The CLI scenarios separately exercise the native regular-file adapter.
add_executable(umicom-source-observation-test
    "${CMAKE_CURRENT_LIST_DIR}/test_source_observation.c" "${CMAKE_CURRENT_LIST_DIR}/fixture.c")
target_link_libraries(umicom-source-observation-test PRIVATE Umicom::distribution)
set_target_properties(umicom-source-observation-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom-source-observation-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-source-observation-test)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-source-observation-test)
endif()
foreach(_case IN ITEMS same changed header duplicate separator context ownership invalid-output
        missing busy io-error malformed oversized file-limit total-limit invalid-options
        cancel-before cancel-between cancel-after unresolved header-metadata empty-inventory
        invalid-path relative-path drive-relative empty-file scale)
    set(_name "framework.build_impact.source_observation.${_case}")
    add_test(NAME "${_name}" COMMAND umicom-source-observation-test "${_case}")
    set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;build-impact;source-observation;regression")
    umicom_test_impact_owners("${_name}" TARGETS umicom-source-observation-test)
endforeach()

# Observed edits feed the established graph selector; no test executable reads
# real source files or runs a compiler while checking these pure service cases.
add_executable(umicom-observed-impact-test
    "${CMAKE_CURRENT_LIST_DIR}/test_observed_impact.c"
    "${CMAKE_CURRENT_LIST_DIR}/fixture.c")
target_link_libraries(umicom-observed-impact-test PRIVATE Umicom::distribution)
set_target_properties(umicom-observed-impact-test PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom-observed-impact-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-observed-impact-test)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-observed-impact-test)
endif()
umicom_target_qualification_inputs(umicom-observed-impact-test FILES
    "${CMAKE_CURRENT_LIST_DIR}/fixture.h")
foreach(_case IN ITEMS same changed-hash changed-size header duplicate-source
        related-tests missing-baseline inventory-changed current-stale outside
        boundary case-difference configuration-change unowned-source
        incomplete-graph malformed-graph null-inventory null-current null-plan
        null-summary occupied-plan ownership all-changed empty-file
        windows-separators unicode scale independent)
    set(_name "framework.build_impact.observed.${_case}")
    add_test(NAME "${_name}" COMMAND umicom-observed-impact-test "${_case}")
    set_tests_properties("${_name}" PROPERTIES TIMEOUT 30
        LABELS "framework;build-impact;source-observation;regression")
    umicom_test_impact_owners("${_name}" TARGETS umicom-observed-impact-test)
endforeach()

# The same public example is also compiled in-tree. These checks do not prove
# installed SDK closure; its independent CMake project checks that separately.
add_executable(umicom-observed-impact-consumer-check
    "${CMAKE_CURRENT_LIST_DIR}/../../examples/observed_impact/main.c")
target_link_libraries(umicom-observed-impact-consumer-check PRIVATE Umicom::distribution)
set_target_properties(umicom-observed-impact-consumer-check PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom-observed-impact-consumer-check)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-observed-impact-consumer-check)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-observed-impact-consumer-check)
endif()
umicom_target_qualification_inputs(umicom-observed-impact-consumer-check FILES
    "${CMAKE_CURRENT_LIST_DIR}/../../examples/observed_impact/check.cmake")
foreach(_case IN ITEMS same source first retry union unknown-retry history-pass history-failed history-unknown-tools saved-pass saved-failed saved-unknown-tools restored-pass restored-failed restored-edited)
    set(_name "framework.build_impact.observed_consumer.${_case}")
    add_test(NAME "${_name}" COMMAND "${CMAKE_COMMAND}"
        "-DPROGRAM=$<TARGET_FILE:umicom-observed-impact-consumer-check>"
        "-DSCENARIO=${_case}"
        "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/observed-consumer/${_case}"
        -P "${CMAKE_CURRENT_LIST_DIR}/../../examples/observed_impact/check.cmake")
    set_tests_properties("${_name}" PROPERTIES TIMEOUT 30
        LABELS "framework;build-impact;consumer;regression")
    umicom_test_impact_owners("${_name}" TARGETS umicom-observed-impact-consumer-check)
endforeach()

# Retry requests are independent of observed byte changes. The same fixture
# ownership graph must remain present when a prior nonpass is the only input.
add_executable(umicom-observed-retries-test
    "${CMAKE_CURRENT_LIST_DIR}/test_observed_retries.c"
    "${CMAKE_CURRENT_LIST_DIR}/fixture.c")
target_link_libraries(umicom-observed-retries-test PRIVATE Umicom::distribution)
set_target_properties(umicom-observed-retries-test PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom-observed-retries-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-observed-retries-test)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-observed-retries-test)
endif()
umicom_target_qualification_inputs(umicom-observed-retries-test FILES
    "${CMAKE_CURRENT_LIST_DIR}/fixture.h")
foreach(_case IN ITEMS unchanged-retry changed-and-retry related missing-baseline
        unknown empty duplicate null-list invalid-name occupied-output null-summary ownership)
    set(_name "framework.build_impact.observed_retries.${_case}")
    add_test(NAME "${_name}" COMMAND umicom-observed-retries-test "${_case}")
    set_tests_properties("${_name}" PROPERTIES TIMEOUT 30
        LABELS "framework;build-impact;source-observation;regression")
    umicom_test_impact_owners("${_name}" TARGETS umicom-observed-retries-test)
endforeach()
