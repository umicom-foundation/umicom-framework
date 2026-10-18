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
    target_sources(umicom PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_impact.c")
    target_link_libraries(umicom PRIVATE Umicom::distribution Umicom::vcs)
    if(BUILD_TESTING)
        add_test(NAME framework.build_impact.cli_help COMMAND umicom impact --help)
        set_tests_properties(framework.build_impact.cli_help PROPERTIES TIMEOUT 20 LABELS "framework;build-impact;cli")
        umicom_test_impact_owners(framework.build_impact.cli_help TARGETS umicom)
        foreach(_case IN ITEMS selected unknown expression no-tests invalid conflicting-options)
            set(_name "framework.build_impact.cli.${_case}")
            add_test(NAME "${_name}" COMMAND "${CMAKE_COMMAND}"
                "-DTOOL=$<TARGET_FILE:umicom>"
                "-DROOT=${CMAKE_CURRENT_LIST_DIR}/.."
                "-DDEST=${CMAKE_CURRENT_BINARY_DIR}/impact-cli-${_case}"
                "-DCASE=${_case}" -P "${CMAKE_CURRENT_LIST_DIR}/../tests/release_impact/check_cli.cmake")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;build-impact;cli;regression")
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
