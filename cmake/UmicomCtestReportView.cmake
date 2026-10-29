# Umicom Framework
# File: cmake/UmicomCtestReportView.cmake
# PURPOSE: Register shared saved-report browsing and focused native qualification.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
include_guard(GLOBAL)

# Filtered presentation does not change the original coverage gate.
target_sources(umicom_testing PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/testing/ctest_coverage_view.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/testing/ctest_coverage_view_text.c")
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/testing/ctest_coverage_view.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/testing" COMPONENT Framework)
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom_testing FILES
        "${CMAKE_CURRENT_LIST_DIR}/../src/testing/ctest_coverage_text_private.h")
endif()
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_testing include/umicom/testing/ctest_coverage_view.h)
endif()

target_sources(umicom_testing PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/testing/ctest_report_view.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/testing/ctest_report_view_text.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/testing/ctest_report_file.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/testing/ctest_comparison_text.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/testing/ctest_comparison_file.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/testing/ctest_coverage.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/testing/ctest_coverage_text.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/testing/ctest_coverage_manifest.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/testing/ctest_coverage_manifest_files.c")
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom_testing FILES
        "${CMAKE_CURRENT_LIST_DIR}/../src/testing/ctest_report_view_private.h"
        "${CMAKE_CURRENT_LIST_DIR}/../src/testing/ctest_coverage_manifest_private.h")
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/testing/ctest_report_view.h"
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/testing/ctest_report_file.h"
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/testing/ctest_comparison_file.h"
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/testing/ctest_coverage.h"
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/testing/ctest_coverage_manifest.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/testing" COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_testing
        include/umicom/testing/ctest_report_view.h include/umicom/testing/ctest_report_file.h
        include/umicom/testing/ctest_comparison_file.h
        include/umicom/testing/ctest_coverage.h include/umicom/testing/ctest_coverage_manifest.h)
endif()
if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/ctest_report.c"
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/ctest_coverage.c"
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/ctest_comparison.c")
    target_link_libraries(umicom_ui_gtk4 PRIVATE Umicom::testing)
    set_property(TARGET umicom_ui_gtk4 PROPERTY UMICOM_CTEST_REPORT_BROWSER_AVAILABLE TRUE)
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/ui/gtk4/ctest_report.h"
        "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/ui/gtk4/ctest_coverage.h"
        "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/ui/gtk4/ctest_comparison.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
    if(COMMAND umicom_release_inventory_own_headers)
        umicom_release_inventory_own_headers(umicom_ui_gtk4 include/umicom/ui/gtk4/ctest_report.h
            include/umicom/ui/gtk4/ctest_coverage.h include/umicom/ui/gtk4/ctest_comparison.h)
    endif()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/guides/saved-test-report-browser.html"
    "${CMAKE_CURRENT_LIST_DIR}/../docs/guides/test-host-coverage.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs" COMPONENT Framework)
# Ship the independent example with both the full and selective SDK. Enumerate
# files explicitly so a developer's consumer build directory is never installed.
foreach(component Framework FrameworkSDK)
    install(FILES
        "${CMAKE_CURRENT_LIST_DIR}/../examples/host_coverage/CMakeLists.txt"
        "${CMAKE_CURRENT_LIST_DIR}/../examples/host_coverage/main.c"
        "${CMAKE_CURRENT_LIST_DIR}/../examples/host_coverage/check.cmake"
        "${CMAKE_CURRENT_LIST_DIR}/../examples/host_coverage/README.md"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/examples/host_coverage"
        COMPONENT ${component})
endforeach()
unset(component)

# Keep the installed comparison example independent of Framework build-tree paths.
foreach(component Framework FrameworkSDK)
    install(FILES
        "${CMAKE_CURRENT_LIST_DIR}/../examples/report_comparison/CMakeLists.txt"
        "${CMAKE_CURRENT_LIST_DIR}/../examples/report_comparison/main.c"
        "${CMAKE_CURRENT_LIST_DIR}/../examples/report_comparison/check.cmake"
        "${CMAKE_CURRENT_LIST_DIR}/../examples/report_comparison/README.md"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/examples/report_comparison"
        COMPONENT ${component})
endforeach()
unset(component)

if(NOT BUILD_TESTING)
    return()
endif()
function(umicom_saved_report_group group owner)
    set(target "umi-saved-report-${group}-test")
    add_executable(${target} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/ctest_report_view/test_${group}.c")
    target_link_libraries(${target} PRIVATE ${owner})
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(${target})
    umicom_apply_sanitizers(${target})
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${target})
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(${target} FILES
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/ctest_report_view/fixture.h"
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/build_log/fixture.h")
    endif()
    # Former condition: if(group STREQUAL "gtk4" AND COMMAND umicom_target_qualification_inputs)
    # Both native report and coverage groups depend on shared GTK fixture inputs.
    if(group MATCHES "gtk4$" AND COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(${target} FILES
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/support/main_context.h")
    endif()
    foreach(case IN LISTS ARGN)
        set(test "framework.ctest.saved_view.${group}.${case}")
        set(work "${CMAKE_CURRENT_BINARY_DIR}/test-work/saved-report/${group}/${case}")
        file(MAKE_DIRECTORY "${work}")
        add_test(NAME ${test} COMMAND ${target} ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 60 WORKING_DIRECTORY "${work}"
            LABELS "framework;testing;saved-report;regression")
        # Former condition: if(group STREQUAL "gtk4")
        # The coverage panel also needs the native host and fatal-warning policy.
        if(group MATCHES "gtk4$")
            set_tests_properties(${test} PROPERTIES SKIP_RETURN_CODE 77 ENVIRONMENT "G_DEBUG=fatal-warnings")
        endif()
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS ${target})
        endif()
    endforeach()
endfunction()
umicom_saved_report_group(filter Umicom::testing default all passed failed timeout skipped disabled not-run none name name-order duration-order outcome-order copied-filter invalid-mask invalid-order invalid-utf8 unterminated null empty large tie-order)
umicom_saved_report_group(format Umicom::testing first middle last empty short query offset bounds unicode controls unchanged-source all-counts)
umicom_saved_report_group(file Umicom::testing valid empty malformed missing relative null directory ownership unicode)
if(TARGET umicom_ui_gtk4)
    umicom_saved_report_group(gtk4 Umicom::ui_gtk4 normal paging filter draft-filter outcomes missing invalid-filter hidden retained cancel-load hide-show close-load close-render captured-path clear captured-filter replace-report cancel-replacement missing-second-page invalid-open-filter cancel-render hide-render hide-clear cancel-page)
endif()

# Coverage consumes saved observations; these checks launch no test subprocess.
umicom_saved_report_group(coverage Umicom::testing mixed passed missing-host missing-test stale
    configuration duplicate-host duplicate-requirement case-sensitive ownership empty null
    unterminated invalid-utf8 capacity multiple-hosts skipped-other-host order failure-output large)

umicom_saved_report_group(coverage_manifest Umicom::testing valid quoted bom crlf ownership empty header-only
    columns unknown duplicate-host duplicate-requirement unused-field blank absolute drive parent
    backslash-parent dot double-separator trailing-separator backslash control invalid-utf8 unterminated capacity null large host-limit)
umicom_saved_report_group(coverage_files Umicom::testing passed skipped missing malformed empty-report stale
    missing-host missing-test configuration ownership relative-base null empty-plan second-invalid unicode subdirectory)

umicom_saved_report_group(coverage_text Umicom::testing outcomes first last empty query short exact
    offset limit null unicode escape ownership source-mismatch)

if(TARGET umicom_ui_gtk4)
    umicom_saved_report_group(coverage_gtk4 Umicom::ui_gtk4 normal paging captured-path captured-identity
        cancel-load hide-show close-load hidden retained replacement invalid-identity relative clear
        close-render reentrant empty stale cancel-replacement cancel-render hide-render hide-clear cancel-page)
endif()

# Compile the same public consumer source during ordinary qualification. This
# in-tree check is distinct from the example's explicit installed-SDK configure.
add_executable(umi-host-coverage-consumer-check
    "${CMAKE_CURRENT_LIST_DIR}/../examples/host_coverage/main.c")
target_link_libraries(umi-host-coverage-consumer-check PRIVATE Umicom::testing)
set_target_properties(umi-host-coverage-consumer-check PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umi-host-coverage-consumer-check)
umicom_apply_sanitizers(umi-host-coverage-consumer-check)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umi-host-coverage-consumer-check)
endif()
foreach(scenario passed skipped stale malformed filtered-passed filtered-empty filtered-text not-run disabled skip-pattern message-controls unicode-path)
    set(test "framework.ctest.coverage_consumer.${scenario}")
    add_test(NAME ${test} COMMAND "${CMAKE_COMMAND}"
        "-DPROGRAM=$<TARGET_FILE:umi-host-coverage-consumer-check>"
        "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/test-work/coverage-consumer/${scenario}"
        "-DSCENARIO=${scenario}"
        -P "${CMAKE_CURRENT_LIST_DIR}/../examples/host_coverage/check.cmake")
    set_tests_properties(${test} PROPERTIES TIMEOUT 30
        LABELS "framework;testing;sdk;public-consumer;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(${test} TARGETS umi-host-coverage-consumer-check umicom_testing)
    endif()
endforeach()
unset(scenario)
unset(test)

# Comparison shares the existing exact-name outcome policy with the CLI.
umicom_saved_report_group(comparison_text Umicom::testing outcomes query short exact offset limit null
    unicode escape ownership first last empty unchanged-failure)
umicom_saved_report_group(comparison_file Umicom::testing valid same empty malformed-before malformed-after
    missing-before missing-after relative-before relative-after null ownership)
if(TARGET umicom_ui_gtk4)
    umicom_saved_report_group(comparison_gtk4 Umicom::ui_gtk4 normal paging captured-before captured-after
        cancel-load hide-show close-load hidden retained replacement-before replacement-after malformed relative
        clear hide-clear cancel-render hide-render close-render reentrant empty unchanged-failure cancel-replacement identity-reload)
endif()

# In-tree checks exercise the same public consumer; installed qualification
# remains a separate configure with an explicit SDK prefix.
add_executable(umi-report-comparison-consumer-check
    "${CMAKE_CURRENT_LIST_DIR}/../examples/report_comparison/main.c")
target_link_libraries(umi-report-comparison-consumer-check PRIVATE Umicom::testing)
set_target_properties(umi-report-comparison-consumer-check PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umi-report-comparison-consumer-check)
umicom_apply_sanitizers(umi-report-comparison-consumer-check)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umi-report-comparison-consumer-check)
endif()
foreach(scenario recovered regressed removed unchanged-failure empty malformed unicode-path)
    set(test "framework.ctest.comparison_consumer.${scenario}")
    add_test(NAME ${test} COMMAND "${CMAKE_COMMAND}"
        "-DPROGRAM=$<TARGET_FILE:umi-report-comparison-consumer-check>"
        "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/test-work/comparison-consumer/${scenario}"
        "-DSCENARIO=${scenario}"
        -P "${CMAKE_CURRENT_LIST_DIR}/../examples/report_comparison/check.cmake")
    set_tests_properties(${test} PROPERTIES TIMEOUT 30
        LABELS "framework;testing;sdk;public-consumer;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(${test} TARGETS umi-report-comparison-consumer-check umicom_testing)
    endif()
endforeach()
unset(scenario)
unset(test)

# Identity checks bind evidence to parsed bytes and preserve failed candidate outputs.
umicom_saved_report_group(file_identity Umicom::testing known empty-suite line-endings malformed
    empty-file missing relative null-output null-identity ownership unicode)
umicom_saved_report_group(comparison_identity Umicom::testing known same semantic malformed-before
    malformed-after missing-before missing-after relative-before relative-after
    null-before null-after null-output null-identity ownership)

# Accepted evidence remains a whole-report observation even when its view is filtered.
if(TARGET umicom_ui_gtk4)
    umicom_saved_report_group(evidence_gtk4 Umicom::ui_gtk4 normal empty-suite mixed filter draft-path
        file-change cancel-replacement missing-replacement clear busy observer hidden owned null
        wrong-widget replacement)
endif()

umicom_saved_report_group(coverage_filter Umicom::testing
    default all passed-gate none empty name capability host configuration combined literal non-ascii
    order-name order-host order-outcome filter-copy outcome-0 outcome-1 outcome-2 outcome-3 outcome-4
    outcome-5 outcome-6 outcome-7 outcome-8 invalid-mask invalid-order unterminated invalid-utf8 controls
    invalid-api format short paging empty-page)

if(TARGET umicom_ui_gtk4)
    umicom_saved_report_group(coverage_filter_gtk4 Umicom::ui_gtk4
        passed outstanding host configuration text capability empty paging edit-render close-render
        clear-render reload failed-reload retained oversize messages)
endif()

# Result messages retain imported evidence without becoming inferred causes.
umicom_saved_report_group(coverage_messages Umicom::testing
    passed failed timeout skip-return skip-pattern disabled not-run empty-message
    unicode controls markup limit stale missing-host missing-test filtered-out)
