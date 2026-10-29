#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomBuildImpact.cmake
# Purpose: Attach the native preview command to existing Framework services.
# Author and Organisation: Sammy Hegab, Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
# This integration owns the helpers it calls, including standalone composition.
include(GNUInstallDirs)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomImpactInventory.cmake")
if(TARGET umicom)

    # Saved qualification inspection reuses Framework parsing; the CLI owns no ledger.
    target_sources(umicom PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_qualification_report.c")
    if(BUILD_TESTING)
        add_test(NAME framework.qualification.archive_cli.help COMMAND umicom qualification-report --help)
        set_tests_properties(framework.qualification.archive_cli.help PROPERTIES TIMEOUT 20
            LABELS "framework;qualification;cli;regression" PASS_REGULAR_EXPRESSION "Historical|Saved evidence")
        umicom_test_impact_owners(framework.qualification.archive_cli.help TARGETS umicom)
        add_executable(umi-qualification-archive-cli-test
            "${CMAKE_CURRENT_LIST_DIR}/../tests/release_qualification/test_archive_cli.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/release_qualification/fixture.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/release_impact/fixture.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_qualification_report.c")
        target_link_libraries(umi-qualification-archive-cli-test PRIVATE Umicom::distribution)
        umicom_target_qualification_inputs(umi-qualification-archive-cli-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/cli.h"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/release_qualification/fixture.h"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/release_impact/fixture.h"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/build_log/fixture.h"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/support/symbolic_link.h")
        set_target_properties(umi-qualification-archive-cli-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(umi-qualification-archive-cli-test)
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(umi-qualification-archive-cli-test)
        endif()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umi-qualification-archive-cli-test)
        endif()
        foreach(_case IN ITEMS passed blocked all receipt wrong-receipt duplicate-option unknown-option relative compare-same compare-removed compare-missing)
            set(_name "framework.qualification.archive_cli.${_case}")
            add_test(NAME "${_name}" COMMAND umi-qualification-archive-cli-test "${_case}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;qualification;cli;regression")
            umicom_test_impact_owners("${_name}" TARGETS umi-qualification-archive-cli-test)
        endforeach()
    endif()
    target_sources(umicom PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_impact.c")
    target_link_libraries(umicom PRIVATE Umicom::distribution Umicom::vcs)
    if(BUILD_TESTING)
        add_test(NAME framework.build_impact.cli_help COMMAND umicom impact --help)
        set_tests_properties(framework.build_impact.cli_help PROPERTIES TIMEOUT 20 LABELS "framework;build-impact;cli")
        umicom_test_impact_owners(framework.build_impact.cli_help TARGETS umicom)
        foreach(_case IN ITEMS selected unknown expression no-tests invalid conflicting-options related related-missing)
            set(_name "framework.build_impact.cli.${_case}")
            add_test(NAME "${_name}" COMMAND "${CMAKE_COMMAND}"
                "-DTOOL=$<TARGET_FILE:umicom>"
                "-DROOT=${CMAKE_CURRENT_LIST_DIR}/.."
                "-DDEST=${CMAKE_CURRENT_BINARY_DIR}/impact-cli-${_case}"
                "-DCASE=${_case}" -P "${CMAKE_CURRENT_LIST_DIR}/../tests/release_impact/check_cli.cmake")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;build-impact;cli;regression")
            umicom_test_impact_owners("${_name}" TARGETS umicom)
        endforeach()


        foreach(_case IN ITEMS single duplicate related-reverse union unknown literal unicode missing-name invalid-option)
            set(_name "framework.build_impact.retry_cli.${_case}")
            add_test(NAME "${_name}" COMMAND "${CMAKE_COMMAND}"
                "-DTOOL=$<TARGET_FILE:umicom>" "-DROOT=${CMAKE_CURRENT_LIST_DIR}/.."
                "-DDEST=${CMAKE_CURRENT_BINARY_DIR}/impact-retry-cli/${_case}"
                "-DCASE=${_case}"
                -P "${CMAKE_CURRENT_LIST_DIR}/../tests/release_impact/check_retry_cli.cmake")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45
                LABELS "framework;build-impact;cli;regression")
            umicom_test_impact_owners("${_name}" TARGETS umicom)
        endforeach()

        # Exercise metadata production and the C23 parser together, preserving
        # an independent exact-name expectation in the fixture driver.
        if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.28")
            foreach(_case IN ITEMS dependency fixture cross-directory missing-dependency)
                set(_name "framework.build_impact.test_graph_cli.${_case}")
                add_test(NAME "${_name}" COMMAND "${CMAKE_COMMAND}"
                    "-DTOOL=$<TARGET_FILE:umicom>"
                    "-DROOT=${CMAKE_CURRENT_LIST_DIR}/.."
                    "-DDEST=${CMAKE_CURRENT_BINARY_DIR}/impact-graph-cli-${_case}"
                    "-DCASE=${_case}" -P "${CMAKE_CURRENT_LIST_DIR}/../tests/release_impact/check_test_graph.cmake")
                set_tests_properties("${_name}" PROPERTIES TIMEOUT 150 LABELS "framework;build-impact;cli;regression")
                umicom_test_impact_owners("${_name}" TARGETS umicom)
            endforeach()
        endif()

        # Source observations are read-only; each case owns its fixture directory.
        foreach(_case IN ITEMS same same-size header missing context unresolved directory invalid-fingerprint duplicate list list-duplicate list-missing)
            set(_name "framework.build_impact.source_observation_cli.${_case}")
            add_test(NAME "${_name}" COMMAND "${CMAKE_COMMAND}"
                "-DTOOL=$<TARGET_FILE:umicom>"
                "-DROOT=${CMAKE_CURRENT_LIST_DIR}/.."
                "-DDEST=${CMAKE_CURRENT_BINARY_DIR}/source-observation-${_case}"
                "-DCASE=${_case}" -P "${CMAKE_CURRENT_LIST_DIR}/../tests/release_impact/check_source_observation.cmake")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 90 LABELS "framework;build-impact;source-observation;cli;regression")
            umicom_test_impact_owners("${_name}" TARGETS umicom)
        endforeach()

        # Real Git runs are restricted to fresh test-owned repositories. These
        # cases exercise discovery without mutating the developer checkout.
        foreach(_case IN ITEMS staged unstaged untracked rename clean since-committed since-mixed since-rename since-gitlink since-invalid since-missing since-blob since-same since-replacement)
            set(_name "framework.build_impact.working_tree.${_case}")
            add_test(NAME "${_name}" COMMAND "${CMAKE_COMMAND}"
                "-DTOOL=$<TARGET_FILE:umicom>"
                "-DROOT=${CMAKE_CURRENT_LIST_DIR}/.."
                "-DDEST=${CMAKE_CURRENT_BINARY_DIR}/impact-working-tree-${_case}"
                "-DCASE=${_case}" -P "${CMAKE_CURRENT_LIST_DIR}/../tests/release_impact/check_working_tree.cmake")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 120 LABELS "framework;build-impact;cli;git;regression")
            umicom_test_impact_owners("${_name}" TARGETS umicom)
        endforeach()
    endif()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/guides/affected-builds.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/guides" COMPONENT Framework)

# VCS owns committed path discovery independently from the CLI presentation.
if(BUILD_TESTING AND TARGET Umicom::vcs)
    include("${CMAKE_CURRENT_LIST_DIR}/../tests/vcs_revision_changes/CMakeLists.txt")
endif()
