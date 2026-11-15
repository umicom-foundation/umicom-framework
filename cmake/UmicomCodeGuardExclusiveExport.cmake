#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCodeGuardExclusiveExport.cmake
#
# PURPOSE:
# Register exclusive report export and native output failure qualification.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
if(BUILD_TESTING)
    set(_export_root "${CMAKE_CURRENT_BINARY_DIR}/tests/codeguard-exclusive-export")
    file(MAKE_DIRECTORY "${_export_root}")
    function(umicom_codeguard_export_target target)
        add_executable(${target} "${CMAKE_CURRENT_LIST_DIR}/../tests/codeguard/test_report_exclusive.c")
        target_link_libraries(${target} PRIVATE Umicom::codeguard)
        target_compile_features(${target} PRIVATE c_std_23)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
    endfunction()
    umicom_codeguard_export_target(umicom-codeguard-export-tests)
    foreach(_case IN ITEMS text json sarif empty-text empty-json empty-sarif existing-text existing-json existing-sarif exact-text exact-json exact-sarif short-text short-json short-sarif zero-budget invalid-format null-result malformed relative dot-path missing-parent unicode large concurrent directory null-path empty-path long-path)
        add_test(NAME "framework.codeguard_export.${_case}"
            COMMAND umicom-codeguard-export-tests "${_case}" "${_export_root}")
        set_tests_properties("framework.codeguard_export.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "codeguard;export;native;ownership")
    endforeach()
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang" AND NOT APPLE AND NOT MSVC)
        umicom_codeguard_export_target(umicom-codeguard-export-fault-tests)
        target_compile_definitions(umicom-codeguard-export-fault-tests PRIVATE UMICOM_CODEGUARD_EXPORT_FAULTS=1)
        target_link_options(umicom-codeguard-export-fault-tests PRIVATE
            -Wl,--wrap=UmiOutputFileCreate -Wl,--wrap=UmiOutputFileWrite -Wl,--wrap=UmiOutputFileClose)
        foreach(_case IN ITEMS create-failure write-failure partial-write close-failure first-error bounded-staging)
            add_test(NAME "framework.codeguard_export_fault.${_case}"
                COMMAND umicom-codeguard-export-fault-tests "${_case}" "${_export_root}")
            set_tests_properties("framework.codeguard_export_fault.${_case}" PROPERTIES
                TIMEOUT 30 LABELS "codeguard;export;native;fault")
        endforeach()
    endif()
endif()

if(BUILD_TESTING)
    add_executable(umicom-codeguard-export-cli-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/codeguard/test_export_cli.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_security.c")
    target_link_libraries(umicom-codeguard-export-cli-tests PRIVATE Umicom::codeguard)
    target_compile_features(umicom-codeguard-export-cli-tests PRIVATE c_std_23)
    umicom_apply_warnings(umicom-codeguard-export-cli-tests)
    umicom_apply_sanitizers(umicom-codeguard-export-cli-tests)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-codeguard-export-cli-tests)
    endif()
    foreach(_case IN ITEMS quality-text quality-json quality-sarif security architecture memory existing relative missing-value duplicate conflict-output conflict-summary unknown-format legacy-output legacy-summary help stdout-new stdout-summary stdout-legacy stdout-report)
        add_test(NAME "framework.codeguard_export_cli.${_case}"
            COMMAND umicom-codeguard-export-cli-tests "${_case}" "${_export_root}")
        set_tests_properties("framework.codeguard_export_cli.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "codeguard;export;cli;compatibility")
    endforeach()
endif()
