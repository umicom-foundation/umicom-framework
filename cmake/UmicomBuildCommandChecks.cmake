#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomBuildCommandChecks.cmake
#
# PURPOSE:
#   Register owned command admission, copying, parsing and display regression cases.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
if(BUILD_TESTING AND TARGET Umicom::build)
    add_executable(umicom-build-command-boundaries-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/build/test_command_boundaries.c")
    target_link_libraries(umicom-build-command-boundaries-test PRIVATE Umicom::build)
    set_target_properties(umicom-build-command-boundaries-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-build-command-boundaries-test)
    umicom_apply_sanitizers(umicom-build-command-boundaries-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-build-command-boundaries-test)
    endif()
    foreach(case IN ITEMS valid empty-program unterminated-program unterminated-directory unterminated-argument excess-count maximum-count empty-argument empty-directory unused-storage stale-cache maximum-fields format-valid format-empty-argument format-quotes format-backslash format-metacharacters format-unicode format-exact format-short format-zero format-null format-count format-program format-directory format-argument copy-valid copy-self copy-stale copy-null-source copy-null-destination copy-invalid copy-independent parse-valid parse-quote parse-empty parse-argument-limit parse-long-argument parse-long-program parse-alias parse-empty-argument parse-null-text parse-null-output)
        add_test(NAME "framework.build.command_boundaries.${case}"
            COMMAND umicom-build-command-boundaries-test "${case}")
        set_tests_properties("framework.build.command_boundaries.${case}" PROPERTIES
            TIMEOUT 20 LABELS "framework;build;argv;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.build.command_boundaries.${case}"
                TARGETS umicom-build-command-boundaries-test)
        endif()
    endforeach()
endif()
